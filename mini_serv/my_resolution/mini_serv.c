#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <signal.h>

static volatile sig_atomic_t	g_should_stop = 0;

enum msgType
{
	LEFT,
	ARRIVED,
	MESSAGE
};

void removeClient(int fd, int *clients, fd_set *allfds);

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void	fatalError(void)
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void broadcastMsg(int *clients, char *msg, enum msgType type, int sender_fd)
{
	char	*full_msg;
	char	tmp[128];
	char	*buffer_copy;
	char	*line;
	int		status;
	int		j;
	int		sender_id = sender_fd - 4;
	
	full_msg = 0;
	if (type == LEFT)
	{
		sprintf(tmp, "server: client %d just left\n", sender_id);
		full_msg = str_join(full_msg, tmp);
		if (full_msg == 0)
			fatalError();
	}
	else if (type == ARRIVED)
	{
		sprintf(tmp, "server: client %d just arrived\n", sender_id);
		full_msg = str_join(full_msg, tmp);
		if (full_msg == 0)
			fatalError();
	}
	else if (type == MESSAGE)
	{
		buffer_copy = str_join(0, msg);
		if (buffer_copy == 0)
			fatalError();
		while ((status = extract_message(&buffer_copy, &line)) > 0)
		{
			sprintf(tmp, "client %d: ", sender_id);
			full_msg = str_join(full_msg, tmp);
			if (full_msg == 0)
				fatalError();
			full_msg = str_join(full_msg, line);
			free(line);
			if (full_msg == 0)
				fatalError();
		}
		if (status == -1)
			fatalError();
		free(buffer_copy);
	}
	if (full_msg == 0)
		return ;
	j = 0;
	while (j < 1024)
	{
		if (clients[j] != -1 && clients[j] != sender_fd && clients[j] != -1)
			send(clients[j], full_msg, strlen(full_msg), 0);
		j++;
	}
	free(full_msg);
}

int addNewClient(int connfd, int *clients)
{
	// Adiciona o novo cliente ao array de clientes e ao conjunto de file descriptors monitorados
	for (int i = 0; i < 1024; i++)
	{
		// Se encontrar um slot disponível no array de clientes, adiciona o novo cliente e retorna
		if (clients[i] == -1)
		{
			clients[i] = connfd;
			broadcastMsg(clients, NULL, ARRIVED, connfd);
			return 0;
		}
	}
	write(2, "Too many clients\n", 17);
	close(connfd);
	return (0);
}

int	identifyNewClient(fd_set *readfds, fd_set *allfds, int sockfd, int *clients, int *maxfd)
{
	// Detectar um  novo cliente:
	if (FD_ISSET(sockfd, readfds))
	{
		struct sockaddr_in cli;
		socklen_t len;
		int connfd;
		len = sizeof(cli);

		// Chama accept() para aceitar a nova conexão do cliente
		connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		if (connfd >= 0)
		{
			if (addNewClient(connfd, clients) == -1)
				return (-1);

			// Adiciona o novo cliente ao conjunto de file descriptors monitorados
			FD_SET(connfd, allfds);
			if (connfd > *maxfd)
				*maxfd = connfd;
		}
	}
	return (0);
}

int checkPendingMessages(fd_set *readfds, int *clients, fd_set *allfds)
{
	// Verifica se há mensagens de clientes existentes
	for (int i = 0; i < 1024; i++)
	{
		int fd = clients[i];
		if (fd == -1)
			continue;

		// Se o cliente tiver enviado uma mensagem, processa a mensagem
		if (FD_ISSET(fd, readfds))
		{
			char buffer[1024];
			int n = recv(fd, buffer, sizeof(buffer) - 1, 0);

			// Se recv() retornar 0 ou um valor negativo, significa que o cliente se desconectou ou ocorreu um erro
			if (n <= 0)
			{
				removeClient(fd, clients, allfds);
				continue;
			}

			// Caso contrário, processa a mensagem recebida (n > 0)
			buffer[n] = '\0'; // Adiciona um terminador de string ao final da mensagem
			broadcastMsg(clients, buffer, MESSAGE, fd);
		}
	}
	return (0);
}

void removeClient(int fd, int *clients, fd_set *allfds)
{
	// Remove o cliente do array de clientes e do conjunto de file descriptors monitorados
	for (int i = 0; i < 1024; i++)
	{
		if (clients[i] == fd)
		{
			close(clients[i]);
			FD_CLR(clients[i], allfds);
			clients[i] = -1;
			broadcastMsg(clients, NULL, LEFT, fd);
			return ;
		}
	}
}

void clearAllFds(fd_set *allfds, int *clients, int sockfd)
{
	for (int i = 0; i < 1024; i++)
	{
		if (clients[i] != -1)
			removeClient(clients[i], clients, allfds);
	}
	close(sockfd);
}

void	main_loop(int sockfd)
{
	// Array para armazenar os file descriptors dos clientes conectados
	int clients[FD_SETSIZE];

	// Inicializa o array de file descriptors dos clientes com -1 (indicando que estão disponíveis)
	for (int i = 0; i < FD_SETSIZE; i++)
		clients[i] = -1;

	// readfds = cópia temporária passada ao select()
	// allfds = conjunto mestre com todos os sockets monitorados
	fd_set readfds, allfds;
	int maxfd;
	
	// Inicializa o conjunto de file descriptors
	FD_ZERO(&allfds);

	// Adiciona o socket do servidor ao conjunto mestre
	FD_SET(sockfd, &allfds);
	maxfd = sockfd;

	// Loop principal do servidor
	while (g_should_stop == 0)
	{
		readfds = allfds;

		// Chama select() para esperar por atividade em qualquer um dos sockets monitorados
		if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				break ;
			write(2, "select error\n", 13);
			break ;
		}

		if (identifyNewClient(&readfds, &allfds, sockfd, clients, &maxfd) == -1)
			break;
		
		if (checkPendingMessages(&readfds, clients, &allfds) == -1)
			break;
	}

	clearAllFds(&allfds, clients, sockfd);
}

void	signalHandler(int sig)
{
	(void)sig;
	g_should_stop = 1;
}

int main(int argc, char *argv[]) {

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		return (1);
	}

	int port = atoi(argv[1]);
	if (port <= 0)
	{
		write(2, "Invalid port\n", 13);
		return (1);
	}
	// fds para o socket do servidor
	int sockfd;

	if (signal(SIGINT, signalHandler) == SIG_ERR)
	{
		write(2, "signal failed\n", 14);
		return (1);
	}
	
	// estruturas para o endereço do servidor
	struct sockaddr_in servaddr; 
	
	// socket create and verification 
	// AF_INET: IPv4, SOCK_STREAM: TCP
	// SOCK_STREAM: tipo de socket
	// 0: protocolo padrão para o tipo de socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		write(2, "Fatal error\n", 12);
		exit(1);
	} 
	else
	write(1, "Socket successfully created..\n", 30);
	
	// zera a estrutura do endereço do servidor
	bzero(&servaddr, sizeof(servaddr)); 
	
	// assign IP, PORT 
	// AF_INET: IPv4
	// htonl: converte um número inteiro para a ordem de bytes da rede
	// htons: converte um short da ordem de bytes da máquina para a ordem de bytes da rede.
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(port); 
	
	// Binding newly created socket to given IP and verification
	// Chama bind() para ligar o socket criado (sockfd) ao endereço do servidor (servaddr).
	// (const struct sockaddr *)&servaddr: endereço convertido para tipo genérico
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		write(2, "socket bind failed...\n", 22);
		exit(1);
	} 
	else
		write(1, "Socket successfully binded...\n", 30);

	// listen: coloca o socket em modo de escuta para aceitar conexões
	// 10: número máximo de conexões pendentes na fila
	if (listen(sockfd, 10) != 0) {
		write(2, "cannot listen\n", 14);
		exit(1);
	}
		
	// // accept: aceita uma conexão de um cliente
	// // (struct sockaddr *)&cli: estrutura para armazenar o endereço do cliente
	// connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	// if (connfd < 0) { 
    //     write(2, "server acccept failed...\n", 25);
    //     exit(1);
    // } 
    // else
    //     write(1, "server acccept the client...\n", 29);

	main_loop(sockfd);
}

// Para compilar e rodar:
// cc -Wall -Wextra -Werror -g mini_serv.c && valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./a.out 4242
