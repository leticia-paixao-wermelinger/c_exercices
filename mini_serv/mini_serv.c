#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

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

void	main_loop(struct sockaddr_in *servaddr, int sockfd, socklen_t len)
{
	// Array para armazenar os file descriptors dos clientes conectados
	int client_fds[1024];

	// Inicializa o array de file descriptors dos clientes com -1 (indicando que estão disponíveis)
	for (int i = 0; i < 1024; i++)
		client_fds[i] = -1;

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
	while (42)
	{
		readfds = allfds;

		// Chama select() para esperar por atividade em qualquer um dos sockets monitorados
		if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			write(2, "select error\n", 13);
			exit(1);
		}

		// Detectar um  novo cliente:
		if (FD_ISSET(sockfd, &readfds))
		{
			connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
			if (connfd >= 0)
			{
				...
			}
		}
	}
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
	// fds para o socket do servidor e para a conexão com o cliente
	int sockfd, connfd;

	
	// estruturas para o endereço do servidor e do cliente
	struct sockaddr_in servaddr, cli; 
	
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
		
	socklen_t len;
	len = sizeof(cli);
	// // accept: aceita uma conexão de um cliente
	// // (struct sockaddr *)&cli: estrutura para armazenar o endereço do cliente
	// connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	// if (connfd < 0) { 
    //     write(2, "server acccept failed...\n", 25);
    //     exit(1);
    // } 
    // else
    //     write(1, "server acccept the client...\n", 29);

	main_loop(&servaddr, sockfd, len);
}

// Para compilar e rodar:
// cc -Wall -Wextra -Werror -g mini_serv.c && valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./a.out 4242