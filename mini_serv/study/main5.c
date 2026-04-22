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

int		g_stop = 0;
int		clients[FD_SETSIZE];
int		max_fds = 0;
fd_set	allFds, selectedFds;

typedef enum s_msgType{
	ENTER,
	LEAVE,
	MSG
} t_msgType;

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

void clearAll(int servFd)
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		close(clients[i]);
		FD_CLR(clients[i], &allFds);
		clients[i] = -1;
	}
	close(servFd);
	FD_ZERO(&allFds);
}

void broadcastMsg(t_msgType msgType, char *msg, int fd)
{
	char *finalMsg = NULL;
	char tmp[FD_SETSIZE];
	int id = fd - 4;

	if (msgType == ENTER)
	{
		sprintf(tmp, "server: client %d just arrived\n", id);
		finalMsg = str_join(finalMsg, tmp);
	}
	else if (msgType == LEAVE)
	{
		sprintf(tmp, "server: client %d just left\n", id);
		finalMsg = str_join(finalMsg, tmp);
	}
	else if (msgType == MSG)
	{
		char *tmpBuff = str_join(NULL, msg);
		char *tmp2 = NULL;
		while (extract_message(&tmpBuff, &tmp2))
		{
			sprintf(tmp, "client %d: %s", id, tmp2);
			finalMsg = str_join(finalMsg, tmp);
			if (tmp2)
			{
				free(tmp2);
				tmp2 = NULL;
			}
		}
		if (tmpBuff)
			free(tmpBuff);
	}

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1 || clients[i] == fd)
			continue ;
		send(clients[i], finalMsg, strlen(finalMsg), MSG_DONTWAIT);
	}
	if (finalMsg)
		free(finalMsg);
}

void insertNewClient(int connfd)
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
		{
			clients[i] = connfd;
			FD_SET(connfd, &allFds);
			broadcastMsg(ENTER, NULL, connfd);
			if (max_fds < connfd)
				max_fds = connfd;
			return ;
		}
	}
}

int checkNewClient(int servFd)
{
	if (FD_ISSET(servFd, &selectedFds))
	{
		int connfd;
		struct sockaddr_in cli;
		unsigned int len = sizeof(cli);

		connfd = accept(servFd, (struct sockaddr *)&cli, &len);

		if (connfd < 0)
			exit(-1);
		
		insertNewClient(connfd);
	}
	return 0;
}

int checkPendingMsgs()
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(clients[i], &selectedFds))
		{
			char buffer[FD_SETSIZE];
			int bytes = recv(clients[i], buffer, FD_SETSIZE - 1, MSG_DONTWAIT);
			if (bytes <= 0)
			{
				broadcastMsg(LEAVE, NULL, clients[i]);
				FD_CLR(clients[i], &allFds);
				close(clients[i]);
				clients[i] = -1;
				return 0;
			}
			buffer[bytes] = '\0';
			broadcastMsg(MSG, buffer, clients[i]);
		}
	}
	return 0;
}

void mainLoop(int servFd)
{
	FD_ZERO(&allFds);
	FD_ZERO(&selectedFds);

	FD_SET(servFd, &allFds);

	for (int i = 0; i < FD_SETSIZE; i++)
		clients[i] = -1;

	max_fds = servFd;

	while (g_stop == 0)
	{
		printf("loop principal\n");
		selectedFds = allFds;
		if (select(max_fds + 1, &selectedFds, NULL, NULL, NULL) < 0)
		{
			printf("erro no select\n");
			if (errno == EINTR)
				continue ;
			fatalError();
			break ;
		}

		if (checkNewClient(servFd) < 0)
			break ;
		
		if (checkPendingMsgs() < 0)
			break ;
	}
	clearAll(servFd);
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