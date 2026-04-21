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

void broadcastMsg(t_msgType msgType, int fd, char *msg)
{
	char *fullMsg = NULL;
	char tmp[FD_SETSIZE];
	int id = fd - 4;

	printf("broadcastMsg");
	if (msgType == ENTER)
	{
		printf("broadcastMsg com fd = %d e com tipo = %s\n", id, "ENTER");
		sprintf(tmp, "server: client %d just arrived\n", id);
		fullMsg = str_join(NULL, tmp);
	}
	else if (msgType == LEAVE)
	{
		printf("broadcastMsg com fd = %d e com tipo = %s\n", id, "LEAVE");
		sprintf(tmp, "server: client %d just left\n", id);
		fullMsg = str_join(NULL, tmp);
	}
	else if (msgType == MSG)
	{
		printf("broadcastMsg com fd = %d e com tipo = %s\n", id, "MSG");
		char *tmp2 = NULL;
		char *buffer_copy = str_join(0, msg);
		while (extract_message(&buffer_copy, &tmp2) > 0)
		{
			sprintf(tmp, "client %d: %s", id, tmp2);
			fullMsg = str_join(fullMsg, tmp);
			if (tmp2)
				free(tmp2);
		}
		if (buffer_copy)
			free(buffer_copy);
	}

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1 || clients[i] == fd)
			continue ;
		send(clients[i], fullMsg, strlen(fullMsg), MSG_DONTWAIT);
	}
	if (fullMsg)
		free(fullMsg);
}

void addNewClient(int connfd)
{
	printf("addNewClient\n");
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
		{
			clients[i] = connfd;
			FD_SET(connfd, &allFds);
			if (max_fds < connfd)
				max_fds = connfd;
			broadcastMsg(ENTER, connfd, NULL);
			return ;
		}
	}
}

int checkNewClient(int servFd)
{
	printf("cleckNewClient\n");
	if (FD_ISSET(servFd, &selectedFds))
	{
		printf("cleckNewClient dentro da validação de FD_ISSET\n");
		int connfd;
		unsigned int len;
		struct sockaddr_in cli; 
		len = sizeof(cli);
		connfd = accept(servFd, (struct sockaddr *)&cli, &len);
		if (connfd < 0) { 
		    fatalError();
			close(servFd);
			exit(1);
		}
		addNewClient(connfd);
	}
	return 0;
}

void removeClient(int fd)
{
	// printf("removeClient\n");
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == fd)
		{
			FD_CLR(fd, &allFds);
			close(fd);
			clients[i] = -1;
			return ;
		}
	}
}

int checkPendingMsgs()
{
	printf("checkPendingMsgs\n");
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		if (FD_ISSET(clients[i], &selectedFds))
		{
			printf("checkPendingMsgs com fd = %d\n", clients[i]);
			char buffer[FD_SETSIZE];
			int bytes = recv(clients[i], buffer, FD_SETSIZE - 1, MSG_DONTWAIT);

			printf("Passou do recv c bytes = %d", bytes);

			if (bytes <= 0)
			{
				// remove client
				broadcastMsg(LEAVE, clients[i], NULL);
				removeClient(clients[i]);
				return 0;
			}

			buffer[bytes] = '\0';
			broadcastMsg(MSG, clients[i], buffer);
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