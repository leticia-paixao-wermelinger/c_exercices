#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>

int g_stop = 0;

typedef enum s_msgType{
	ENTER,
	LEAVE,
	MSG
} t_msgType;

void debug(char *txt)
{
	printf("DBG: %s\n", txt);
}

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

void fatalError()
{
	write(1, "Fatal error\n", 12);
}

void	sigHandle(int sig)
{
	if (sig == SIGINT)
		g_stop = 1;
}

void clearAll(int *clients, int servSockfd, fd_set *fds)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		close(clients[i]);
		clients[i] = -1;
	}
	FD_ZERO(fds);
	close(servSockfd);
}

void broadcastMsg(t_msgType msgType, char *msg, int *clients, int connfd)
{
	char *fullMsg = NULL;
	char tmpMsg[FD_SETSIZE];
	char *line = NULL;
	int id = connfd - 4;

	if (msgType == ENTER) // server: client %d just arrived\n
	{
		sprintf(tmpMsg, "server: client %d just arrived\n", id);
		fullMsg = str_join(NULL, tmpMsg);
	}
	else if (msgType == LEAVE)
	{
		// debug("em broadcast, entrou na validação de remoção de cliente");
		sprintf(tmpMsg, "server: client %d just left\n", id);
		fullMsg = str_join(NULL, tmpMsg);
		// debug("full message:");
		// debug(fullMsg);
	}
	else if (msgType == MSG) // client %d: msg
	{
		//debug("entrou na validação de mensagem");
		while (extract_message(&msg, &line) == 1)
		{
			sprintf(tmpMsg, "client %d: %s", id, line);
			fullMsg = str_join(fullMsg, tmpMsg);
		}
		if (line)
		{
			free(line);
			line = NULL;
		}
		if (msg)
		{
			free(msg);
			msg = NULL;
		}
	}

	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == connfd || clients[i] == -1)
			continue ;
		if (!fullMsg)
			continue ;
		if (send(clients[i], fullMsg, strlen(fullMsg), MSG_DONTWAIT) == -1)
			debug("erro de envio de mensagem");
	}
	free(fullMsg);
}

void	addNewClient(int *clients, fd_set *allFds, int connFd)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
		{
			clients[i] = connFd;
			FD_SET(connFd, allFds);
			broadcastMsg(ENTER, NULL, clients, connFd);
			return ;
		}
	}
	// debug("Não foi possível criar o cliente");
}

int checkNewClient(int sockFd, fd_set *selectedFds, int *clients, fd_set *allFds)
{
	if (FD_ISSET(sockFd, selectedFds))
	{
		int connfd;
		struct sockaddr_in cli; 
		unsigned int len;
		len = sizeof(cli);
		connfd = accept(sockFd, (struct sockaddr *)&cli, &len);
		if (connfd < 0) { 
		    fatalError();
			return -1;
		} 
		else
		{
		    write(1, "server acccept the client...\n", 29);
			addNewClient(clients, allFds, connfd);
			return 1;
		}
		close(sockFd);
	}

	return 0;
}

void	removeClient(int *clients, int connfd, fd_set *allFds)
{
	// debug("em remove client");
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == connfd)
		{
			clients[i] = -1;
			FD_CLR(connfd, allFds);
			close(connfd);
			break ;
		}
	}
}

int checkPendingMsgs(int *clients, fd_set *selectedFds, fd_set *allFds)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		if (FD_ISSET(clients[i], selectedFds))
		{
			char buffer[FD_SETSIZE];

			int bytes = recv(clients[i], buffer, FD_SETSIZE, MSG_DONTWAIT);
			if (bytes == 0)
			{
				int fd = clients[i];
				// client disconected
				// debug("var remover cliente");
				removeClient(clients, fd, allFds);
				// debug("vai fazer o broadcast do cliente removido");
				broadcastMsg(LEAVE, NULL, clients, fd);
				return 0;
			}
			else if (bytes < 0)
				break ;
			buffer[bytes] = '\0';
			broadcastMsg(MSG, buffer, clients, clients[i]);
		}
	}

	return 0;
}

void	mainLoop(int servSockFd)
{
	int clients[FD_SETSIZE];
	fd_set allFds, selectedFd;
	int maxFds = servSockFd;

	for (int i = 0; i <= FD_SETSIZE; i++)
		clients[i] = -1;

	FD_ZERO(&allFds);

	FD_SET(servSockFd, &allFds);

	while (g_stop == 0)
	{
		selectedFd = allFds;
		if (select(maxFds + 1, &selectedFd, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue ;
			fatalError();
			break ;
		}

		int new = checkNewClient(servSockFd, &selectedFd, clients, &allFds);
		if (new < 0)
			break ;
		else if (new == 1)
			maxFds++;
				
		if (checkPendingMsgs(clients, &selectedFd, &allFds) < 0)
			break ;
	}
	clearAll(clients, servSockFd, &allFds);
}

int main(int argc, char *argv[]) {
	int sockfd;//, connfd;
	//unsigned int len;
	struct sockaddr_in servaddr;//, cli; 

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		return 1;
	}

	signal(SIGINT, sigHandle);

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) {
		fatalError(); 
		exit(1);
	}
	else
		write(1, "Socket successfully created..\n", 30); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		fatalError();
		exit(1);
	} 
	else
		write(1, "Socket successfully binded..\n", 30);

	if (listen(sockfd, 10) != 0) {
		fatalError(); 
		exit(1);
	}

	mainLoop(sockfd);
	// len = sizeof(cli);
	// connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	// if (connfd < 0) { 
    //     fatalError();
	// 	clearServer(&sockfd);
	// 	exit(1);
    // } 
    // else
    //     write(1, "server acccept the client...\n", 29);
	// clearServer(&sockfd);
}