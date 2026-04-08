#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>

int boolStop = 0;

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

void	main_loop(int sockfd, struct sockaddr_in servaddr, struct sockaddr_in cli)
{
	int connfds[1842];

	int i = 0;
	while (i <= 1842)
		connfds[i] = -1;
	
	while (boolStop == 0)
	{	
		if ()
		unsigned int len = sizeof(cli);
		connfds[i] = accept(sockfd, (struct sockaddr *)&cli, &len);
		if (connfd < 0) { 
			write(2, "Fatal error\n", 12);
			exit(1); 
		}
	}
	clearAllFDs(connfds);
}

void	stopServer(int sig)
{
	if (sig == SIGINT)
		boolStop = 1;
}

int main(int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in servaddr, cli; 

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		return 1;
	}

	signal(SIGINT, stopServer);

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		write(2, "Fatal error\n", 12);
		exit(1); 
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
		exit(1); 
	}
	else
		write(1, "Socket successfully binded...\n", 30);
	
	if (listen(sockfd, 10) != 0) {
		write(2, "Fatal error\n", 12);
		exit(1);
	}

	// unsigned int len = sizeof(cli);
	// connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	// if (connfd < 0) { 
    //     write(2, "Fatal error\n", 12);
	// 	exit(1); 
    // } 
    
	// write(1, "server acccept the client...\n", 29);

	main_loop(sockfd, servaddr, cli);

	return 0;
}