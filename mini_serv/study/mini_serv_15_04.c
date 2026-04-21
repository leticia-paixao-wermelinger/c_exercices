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

enum s_msgType{
	ENTER,
	LEAVE,
	MSG
}	typedef t_msgType;

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
			free(newbuf);
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

void	clearServer(int sockfd)
{
	close(sockfd);
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
	clearServer(servSockfd);
}

void	broadcastMsg(char *msg, t_msgType type, int *clients, int connfd)
{
	char *fullMsg = NULL;
	char tmp[FD_SETSIZE];
	char *tmp2 = NULL;
	int id = connfd - 4;
	if (type == ENTER) // server: client %d just arrived\n
	{
		sprintf(tmp, "server: client %d just arrived\n", id);
		fullMsg = str_join(fullMsg, tmp);
	}
	else if (type == LEAVE) // server: client %d just left\n
	{
		sprintf(tmp, "server: client %d just left\n", id);
		fullMsg = str_join(fullMsg, tmp);
	}
	else if (type == MSG) // client %d: {line}
	{
		// int i = 0;
		while (extract_message(&msg, &tmp2))
		{
			// printf("msg = %s\n", msg);
			// printf("tmp = %s\n", tmp2);
			// i++;
			sprintf(tmp, "client %d: %s", id, tmp2);
			fullMsg = str_join(fullMsg, tmp);
		}
		
	}

	// printf("fullmsg = %s\n", fullMsg);
	// printf("strlen de fullmsg = %lu\n", strlen(fullMsg));
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients [i] == -1 || clients[i] == connfd)
			continue;
		send(clients[i], fullMsg, strlen(fullMsg), MSG_DONTWAIT);
	}
	free(fullMsg);
	free(tmp2);
	free(msg);
}

void	insertNewClient(int *clients, fd_set *allFds, int connfd)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
		{
			printf("teste com i = %i clients[i] = %d\n", i, clients[i]);
			clients[i] = connfd;
			FD_SET(connfd, allFds);
			broadcastMsg(NULL, ENTER, clients, connfd);
			return ;
		}
	}
	write(2, "maximum fds reached\n", 20);
}

int checkNewClient(fd_set *selectFds, fd_set *allFds, int servSockfd, int *clients)
{
	// debug("newclient");
	if (FD_ISSET(servSockfd, selectFds))
	{
		int connfd;
		struct sockaddr_in cli; 
		unsigned int len = sizeof(cli);
		connfd = accept(servSockfd, (struct sockaddr *)&cli, &len);
		if (connfd < 0) { 
		    fatalError();
		 	clearAll(clients, servSockfd, allFds);
		 	exit(1);
		} 
		// debug("Vai inserir cliente");
		insertNewClient(clients, allFds, connfd);
	}
	return 0;
}

void	removeClient(int *clients, int rmvFd, fd_set *allFds)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == rmvFd)
		{
			close(rmvFd);
			clients[i] = -1;
			FD_CLR(rmvFd, allFds);
//			(void)allFds;
			return ;
		}
	}
}

int checkPendingMsgs(int *clients, fd_set *selectFds, fd_set *allFds)
{
	// debug("pendingmsgs");
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] == -1)
			continue ;
		if (FD_ISSET(clients[i], selectFds))
		{
			char buffer[FD_SETSIZE];
			if (recv(clients[i], buffer, sizeof(buffer), MSG_DONTWAIT) == 0)
			{
				//debug("recv retornou 1");
				broadcastMsg(NULL, LEAVE, clients, clients[i]);
				removeClient(clients, clients[i], allFds);
				return 0;
			}
			broadcastMsg(buffer, MSG, clients, clients[i]);
		}
	}

	return 0;
}

void	mainLoop(int servSockfd)
{
	int clients[FD_SETSIZE];
	int maxFds = servSockfd;
	fd_set allFds, selectFds;

	FD_ZERO(&allFds);

	FD_SET(servSockfd, &allFds);

	for (int i = 0; i <= FD_SETSIZE; i++)
		clients[i] = -1;

	// debug("Vai entrar no loop");
	while (g_stop == 0)
	{
		selectFds = allFds;
		// debug("no loop");

		if (select(maxFds + 1, &selectFds, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue;
			fatalError();
			break ;
		}
		// debug("apos select");

		if (checkNewClient(&selectFds, &allFds, servSockfd, clients) < 0)
			break ;

		maxFds++;
		// debug("apos new client");
		if (checkPendingMsgs(clients, &selectFds, &allFds) < 0)
			break ;
	}
	clearAll(clients, servSockfd, &allFds);
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