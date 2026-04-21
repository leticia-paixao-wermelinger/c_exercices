#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

int close_server = 0;

enum	e_msg_code
{
	NEW_CLIENT,
	LOST_CLIENT,
	MSG
}	typedef	t_msg_code;

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

void clear_all(int sockfd, int *clients)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		// #include<stdio.h>
		// printf("Vai limpar clients[%d] = %d", i, clients[i]);
		if (clients[i] == -1)
			continue ;
		close(clients[i]);
		clients[i] = -1;
	}
	close(sockfd);
}

void	fatal_error()
{
	write(2, "Fatal error\n", 12);
}

void test(char *msg)
{
	#include<stdio.h>
	printf("teste: %s\n", msg);
}

void	broadcast_msg(t_msg_code type, char *buffer, int sockfd)
{
	char *msg;
	char *tmp;
	if (type == NEW_CLIENT)
	{}
	else if (type == LOST_CLIENT)
	{}
	else if (type == MSG)
	{
		tmp = str_join("client: ", buffer);
		msg = str_join(tmp, "\0");
		free(tmp);
	}
	send(sockfd, msg, sizeof(msg), MSG_DONTWAIT);
}

void	add_new_client(int *clients, int connfd, int sockfd, fd_set all_fds)
{
	for (int i = 0; i <= FD_SETSIZE; i++)
	{
		if (clients[i] != connfd && clients[i] != -1)
			continue ;
		if (clients[i] == connfd)
			break ;
		clients[i] = connfd;
		FD_SET(sockfd, &all_fds);
		write(2, "server acccept the client...\n", 29);
		// broadcast_msg(NEW_CLIENT);
		break ;
	}

}

int check_new_client(fd_set selected_fds, int sockfd, int *clients)
{
	test("check_new_client");
	if (FD_ISSET(sockfd, &selected_fds))
	{
		struct sockaddr_in cli;
		unsigned int len;
		int connfd;
		len = sizeof(cli);

		connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		if (connfd < 0)
		{
			fatal_error();
			return -1;
		}
		else
			add_new_client(&clients[0], connfd, sockfd, selected_fds);
	}
	return 0;
}

int check_msg(int sockfd, int *clients, fd_set selected_fds)
{
	test("check_msg");
	(void)clients;
	if (FD_ISSET(sockfd, &selected_fds))
	{
		// struct sockaddr_in cli;
		// int connfd;

		char buffer[FD_SETSIZE];
		buffer[FD_SETSIZE - 1] = '\0';

		if (recv(sockfd, buffer, FD_SETSIZE, MSG_DONTWAIT) < 0)
		{
			remove_client();
			return 1;
		}
		broadcast_msg(MSG, buffer, sockfd);
	}
	return 0;
}

void	main_loop(int sockfd)
{
	int clients[FD_SETSIZE];
	for (int i = 0; i <= FD_SETSIZE; i++)
		clients[i] = -1;

	int max_fds = sockfd;

	fd_set all_fds, selected_fds;
	FD_ZERO(&all_fds);
	FD_SET(sockfd, &all_fds);

	while (close_server == 0)
	{
		test("loop");
		selected_fds = all_fds;
		if (select(max_fds + 1, &selected_fds, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				break ;
			fatal_error();
			break ;
		}
		#include<stdio.h>
		printf("selected_fds[0] = ", selected_fds)
		if (check_new_client(selected_fds, sockfd, &clients[0]) < 0)
			break ;
		if (check_msg(sockfd, clients, selected_fds) < 0)
			break ;
	}
	clear_all(sockfd, clients);
}

// void checkErrno(int bindRet)
// {
// 	#include <stdio.h>
// 	if (bindRet == EACCES)
// 		printf("EACCES");
// 	else if (bindRet == EADDRINUSE)
// 		printf("EADDRINUSE");
// 	else if (bindRet == EBADF)
// 		printf("EBADF");
// 	else if (bindRet == EINVAL)
// 		printf("EINVAL");
// 	else if (bindRet == ENOTSOCK)
// 		printf("ENOTSOCK");
// 	else if (bindRet == EADDRNOTAVAIL)
// 		printf("EADDRNOTAVAIL");
// 	else if (bindRet == EFAULT)
// 		printf("EFAULT");
// 	else if (bindRet == ELOOP)
// 		printf("ELOOP");
// 	else if (bindRet == ENAMETOOLONG)
// 		printf("ENAMETOOLONG");
// 	else if (bindRet == ENOENT)
// 		printf("ENOENT");
// 	else if (bindRet == ENOMEM)
// 		printf("ENOMEM");
// 	else if (bindRet == ENOTDIR)
// 		printf("ENOTDIR");
// 	else if (bindRet == EROFS)
// 		printf("EROFS");
// 	else
// 		printf("Other error: bindRet = %d", bindRet);
// }

void handle_sig(int sig)
{
	if (sig == SIGINT)
		close_server = 1;
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

	signal(SIGINT, handle_sig);

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
	{
		fatal_error();
		return 1;
	}
	else
		write(1, "Socket successfully created..\n", 30); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 

	// write(1, "to aqui\n", 8);
	// Binding newly created socket to given IP and verification 
	int bindRet = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
	if (bindRet != 0)
	{
		fatal_error();
		return 1;
	}
	else
		write(1, "Socket successfully binded..\n", 29);

	if (listen(sockfd, 10) != 0)
	{
		fatal_error();
		return 1;
	}

	main_loop(sockfd);
	// len = sizeof(cli);
	// connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	// if (connfd < 0)
	// 	fatal_error(sockfd);
    // else
    //     write(2, "server acccept the client...\n", 29);
}