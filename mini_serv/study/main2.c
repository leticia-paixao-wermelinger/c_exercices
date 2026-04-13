#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

int ctrl_C = 0;

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

void signalHandler(int signum)
{
	if (signum == SIGINT)
		ctrl_C = 1;
}

int newClient (int *clients, fd_set *readFDs, int *maxfd)
{
	int i = 0;

	if (FD_ISSET(clients[0], readFDs))
	{
		struct sockaddr *addr;
		unsigned int len = sizeof(addr);
		int currfd = accept(clients[0], addr, len);
		
		FD_SET(currfd, readFDs);
		if (currfd > maxfd)
			*maxfd = currfd;
	}
	return 0;
}

int sendMgs()
{
	return 0;
}

void	mainLoop(int sockfd, unsigned int len, struct sockaddr_in servaddr, struct sockaddr_in cli)
{
	int numberFDs = 1;
	fd_set *allFDs, *readFDs;
	int clients[FD_SETSIZE];

	clients[0] = sockfd;

	int i = 1;
	while (i < FD_SETSIZE)
	{
		clients[i] = -1;
		i++;
	}

	FD_ZERO(allFDs);

	FD_SET(sockfd, allFDs);

	while (ctrl_C == 0)
	{
		readFDs = allFDs;
		int retSelect = select(numberFDs, readFDs, NULL, NULL, NULL);
		if (retSelect < 0)
			break ;

		if (newClient(clients, readFDs, &numberFDs) < 0)
			break ;
		if (sendMsg() < 0)
			break ;
	}
//	cleanAll();
}

int main(int argc, char *argv[]) {

	if (argc != 2)
	{
		write(2, "Fatal error\n", 12);
		return 1;
	}

	int sockfd;
	unsigned int len;
	struct sockaddr_in servaddr, cli; 

	signal(SIGINT, signalHandler);

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		write(2, "Fatal error\n", 12);
		return 1;
	} 
	else
		write(1, "Socket successfully created...\n", 31); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		write(2, "Fatal error\n", 12);
		return 1; 
	} 
	else
		write(1, "Socket successfully binded...\n", 30);
	if (listen(sockfd, 10) != 0) {
		write(2, "Fatal error\n", 12);
		return 1; 
	}

	mainLoop(sockfd, len, servaddr, cli);
}