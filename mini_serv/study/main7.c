#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

typedef enum s_typeMessage
{
	ARRIVE,
	LEAVE,
	MSG
}	t_msgType;

unsigned long int max_buffer = 20000;

typedef struct s_client
{
	int fd;
	int id;
	char *buffer;
} t_client;

int 		sockfd;
int			max_fd = 0;
fd_set 		all_clients, selected_clients;
t_client	clients[FD_SETSIZE];

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

void	broadcastMsg(t_msgType msgType, char *recvMsg, t_client client)
{
	char tmp[max_buffer];
	char *finalMsg = NULL;

	printf("Chegou na broadcastMsg ");
	if (msgType == ARRIVE)
	{
		printf("com msgType = ARRIVE.");
		sprintf(tmp, "server: client %d just arrived\n", client.id);
		finalMsg = str_join(finalMsg, tmp);
	}
	else if (msgType == LEAVE)
	{
		printf("com msgType = LEAVE.");
		sprintf(tmp, "server: client %d just left\n", client.id);
		finalMsg = str_join(finalMsg, tmp);
	}
	else if (msgType == MSG)
	{
		printf("com msgType = MSG e com msg = |%s|", recvMsg);
		char *partialMsg = NULL;
		if (client.buffer)
			finalMsg = str_join(NULL, client.buffer);
		free(client.buffer);
		client.buffer = NULL;
		while (extract_message(&recvMsg, &partialMsg))
		{
			sprintf(tmp, "client %d: %s", client.id, partialMsg);
			finalMsg = str_join(finalMsg, tmp);
		}
		if (strlen(recvMsg) != 0)
			strcpy(client.buffer, recvMsg);
	}

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id == -1)
			continue ;
		else if (clients[i].id == client.id)
			continue ;
		send(clients[i].fd, finalMsg, strlen(finalMsg), 0);
	}
}

void remove_client(t_client client)
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id == client.id)
		{
			broadcastMsg(LEAVE, NULL, client);
			FD_CLR(clients[i].fd, &all_clients);
			clients[i].fd = -1;
			clients[i].id = -1;
			clients[i].buffer = NULL;
			return ;
		}
	}
}

int	checkNewMsg()
{
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(clients[i].fd, &selected_clients))
		{
			printf("No loop da checkNewMsg. fd do client = %i\n", clients[i].fd);
			char buffer[max_buffer];
			int bufferSize = recv(clients[i].fd, buffer, max_buffer - 1, 0);
			if (bufferSize <= 0)
			{
				remove_client(clients[i]);
				return 0;
			}
			buffer[bufferSize] = '\0';
			broadcastMsg(MSG, buffer, clients[i]);
		}
	}
	return 0;
}

void	addNewClient(int connfd)
{
	static int last_id = -1;
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].fd == -1)
		{
			printf("vai add cliente no indice %i\n", i);
			last_id++;
			clients[i].fd = connfd;
			clients[i].id = last_id;
			clients[i].buffer = NULL;
			if (max_fd < connfd)
				max_fd = connfd;
			broadcastMsg(ARRIVE, NULL, clients[i]);
			FD_SET(connfd, &all_clients);
			return ;
		}
	}
}

int checkNewClient()
{
	if (FD_ISSET(sockfd, &selected_clients))
	{
		int connfd;
		unsigned int len;
		struct sockaddr_in cli; 
		
		len = sizeof(cli);
		connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		printf("Na checkNewClient. fd do client = %i\n", connfd);
		if (connfd < 0)
			fatalError();
		addNewClient(connfd);
	}
	return 0;
}

void main_loop()
{
	while (42)
	{
		// printf("No início do loop\n");
		selected_clients = all_clients;
		if (select(max_fd + 1, &selected_clients, NULL, NULL, NULL) < 0)
			fatalError();
		// printf("Vai p checkNewMsg\n");
		if (checkNewMsg() < 0)
			fatalError();
		// printf("Vai p checkNewClient\n");
		if (checkNewClient() < 0)
			fatalError();
	}
}

int main(int argc, char *argv[]) {
	// int connfd;
	// unsigned int len;
	struct sockaddr_in servaddr; 

	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

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

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		clients[i].fd = -1;
		clients[i].id = -1;
		clients[i].buffer = NULL;
	}

	FD_ZERO(&all_clients);
	FD_ZERO(&selected_clients);

	FD_SET(sockfd, &all_clients);

	max_fd = sockfd;

	main_loop();
}