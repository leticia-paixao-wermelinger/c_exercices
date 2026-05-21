#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int clients[FD_SETSIZE];
/* MODIFICADO: guardamos o id real de cada fd; id nao pode ser calculado por fd - 4,
   porque o sistema pode reutilizar fds depois que clientes saem. */
int clientIds[FD_SETSIZE];
char *bufferFds[FD_SETSIZE];
fd_set allFds, selectedFds;
int	maxFds = 0;
/* MODIFICADO: contador monotonicamente crescente, como o subject pede. */
int nextClientId = 0;

int sockfd;

typedef enum s_msgType{
	NEW,
	LEFT,
	MSG
}	t_msgType;

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

void	fatalError()
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void broadcastMsg(t_msgType msgType, char *msg, int fd)
{
	/* MODIFICADO */
	int id = clientIds[fd];

	char tmp[FD_SETSIZE];
	char *finalMsg = NULL;

	if (msgType == NEW)
	{
		sprintf(tmp, "server: client %d just arrived\n", id);
		finalMsg = str_join(finalMsg, tmp);
		if (finalMsg == NULL)
			fatalError();
	}
	else if (msgType == LEFT)
	{
		sprintf(tmp, "server: client %d just left\n", id);
		finalMsg = str_join(finalMsg, tmp);
		if (finalMsg == NULL)
			fatalError();
	}
	else if (msgType == MSG)
	{
		char *tmpHelp = NULL;
		while (42)
		{
			/* MODIFICADO: msg ja deve ser o buffer acumulado do cliente.
			   Aqui so extraimos linhas completas; a sobra fica em bufferFds[fd]. */
			int ret = extract_message(&msg, &tmpHelp);
			if (ret == -1)
				fatalError();
			if (ret == 1)
			{
				char prefix[64];
				/* MODIFICADO: nao montamos prefixo + linha em um array fixo.
				   Uma linha recebida pode ser maior que FD_SETSIZE. */
				sprintf(prefix, "client %d: ", id);
				finalMsg = str_join(finalMsg, prefix);
				if (finalMsg == NULL)
					fatalError();
				finalMsg = str_join(finalMsg, tmpHelp);
				if (finalMsg == NULL)
					fatalError();
				free(tmpHelp);
				tmpHelp = NULL;
			}
			if (ret == 0)
				break ;
		}
		/* MODIFICADO: extract_message altera o ponteiro do buffer para apontar
		   para a sobra depois do ultimo '\\n'. Precisamos salvar isso de volta. */
		bufferFds[fd] = msg;
	}

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		if (clients[i] == fd)
			continue ;
		/* MODIFICADO: bufferFds eh entrada incompleta recebida de cada cliente,
		   nao uma fila de saida para enviar depois. Enviamos so finalMsg. */
		if (finalMsg)
			send(clients[i], finalMsg, strlen(finalMsg), 0);
	}
	
	if (finalMsg)
		free(finalMsg);
}

void	addNewClient(int fd)
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
		{
			clients[i] = fd;
			/* MODIFICADO: atribui id crescente, independente do fd. */
			clientIds[fd] = nextClientId++;
			FD_SET(fd, &allFds);
			if (maxFds < fd)
				maxFds = fd;
			broadcastMsg(NEW, NULL, fd);
			return ;
		}
	}
	// q q faz se não tiver espaço pro cliente? não lembro
}

int checkNewClient()
{
	if (FD_ISSET(sockfd, &selectedFds))
	{
		unsigned int len;
		int connfd;
		struct sockaddr_in cli; 

		len = sizeof(cli);
		connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		if (connfd < 0)
			fatalError();

		addNewClient(connfd);
	}
	return 0;
}

int checkNewMsg()
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		/* MODIFICADO: primeiro checa se ha cliente valido; FD_ISSET(-1, ...)
		   tem comportamento indefinido. */
		if (clients[i] == -1)
			continue ;
		if (FD_ISSET(clients[i], &selectedFds))
		{
			char buffer[FD_SETSIZE];

			int bufferSize = recv(clients[i], buffer, FD_SETSIZE - 1, 0);

			if (bufferSize <= 0)
			{
				int fd = clients[i];
				broadcastMsg(LEFT, NULL, fd);
				/* MODIFICADO: limpa a sobra pendente desse cliente para evitar leak. */
				free(bufferFds[fd]);
				bufferFds[fd] = NULL;
				clients[i] = -1;
				FD_CLR(fd, &allFds);
				close(fd);
				return 0;
			}

			buffer[bufferSize] = '\0';
			/* MODIFICADO: junta o recv atual com a sobra anterior do mesmo fd
			   antes de tentar extrair linhas completas. */
			bufferFds[clients[i]] = str_join(bufferFds[clients[i]], buffer);
			if (bufferFds[clients[i]] == NULL)
				fatalError();
			broadcastMsg(MSG, bufferFds[clients[i]], clients[i]);
		}
	}
	return 0;
}

void mainLoop()
{
	while (42)
	{
		selectedFds = allFds;

		if (select(maxFds + 1, &selectedFds, NULL, NULL, NULL) < 0)
			fatalError();

		/* MODIFICADO: processa mensagens/desconexoes antes de aceitar novos clientes.
		   Isso evita enviar "arrived" para sockets que ja fecharam no mesmo select. */
		if (checkNewMsg() < 0)
			break ;

		if (checkNewClient() < 0)
			break;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		return 1;
	}

	struct sockaddr_in servaddr; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) 
		fatalError(); 

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) 
		fatalError();

	if (listen(sockfd, 10) != 0) 
		fatalError();

	maxFds = sockfd;

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		clients[i] = -1;
		/* MODIFICADO: inicializa tabela de ids junto com a tabela de clientes. */
		clientIds[i] = -1;
		bufferFds[i] = NULL;
	}

	FD_ZERO(&allFds);
	FD_ZERO(&selectedFds);

	FD_SET(sockfd, &allFds);

	mainLoop();
}
