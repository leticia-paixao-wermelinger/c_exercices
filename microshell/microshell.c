#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void    execution(char **av, int i, char **env);
void    do_cd(char **av, int i, int flag);
void    print_error(char *str);
void    run_execve(char **av, char **env);
void	set_pipe(char last, int *pipefd, int fd);

int	main(int argc, char **argv, char **envp)
{
	int	i = 0;

	(void)argc;
	argv++;
	while (argv[i])
	{
//		printf("No loop da main com argv[%i] = %s\n", i, argv[i]);
		if (strcmp(argv[i], "\0") != 0 && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			i++;
		else if (strcmp(argv[i - 1], ";") == 0 && strcmp(argv[i], ";") == 0)
		{
			argv++;
			i = 0;
		}
		else if (i)
		{
			execution(argv, i, envp);
			argv += i + 1;
			i = 0;
		}
		else
			i++;
	}
	if (i)
		execution(argv, i - 1, envp);
//	printf("Saiu do loop da main com argv[%i] = %s\n", i, argv[i]);
}

void	execution(char **av, int i, char **env)
{
	int pipefd[2];
	pid_t cpid;
	int *wstatus = NULL;

//	printf("Execução recebeu av[0] = %s\n", av[0]);
//	printf("Execução recebeu av[%i] = %s\n", i, av[i]);
	if (strcmp(av[i], "|") != 0 && strcmp(av[0], "cd") == 0)
	{
		if (strcmp(av[i], ";") == 0)
			do_cd(av, i - 1, 1);
		else
			do_cd(av, i, 2);
		return ;
	}
	else if (av[i][0] == '|')
	{
		if (pipe(pipefd) < 0)
		{
			print_error("error: fatal\n");
			exit(1);
		}
	}
//	printf("Vai forkar\n");
	cpid = fork();
	if (cpid < 0)
	{
		print_error("error: fatal\n");
		exit(1);
	}
	if (cpid == 0)
	{
	//	printf("No processo filho, com av[0] = %s\n", av[0]);
		set_pipe(av[i][0], pipefd, 1);
	//	printf("A. Antes de run exec, com av[0] = %s e av[%i] = %s\n", av[0], i, av[i]);
		if (av[i][0] == '|' || av[i][0] == ';')
			av[i] = NULL;
	//	printf("B. Antes de run exec, com av[0] = %s e av[%i] = %s\n", av[0], i, av[i]);
		run_execve(av, env);
	}
	else if (cpid > 0)
		waitpid(cpid, wstatus, 0);
//	printf("voltou p o processo pai\n");
	set_pipe(av[i][0], pipefd, 0);
	return ;
}

void	do_cd(char **av, int j, int flag)
{
	if (j != 1 && flag == 1)
	{
		print_error("error: cd: bad arguments\n");
		return ;
	}
	else if (j != 1 && flag == 2)
		return ;
	if (chdir(av[1]) == -1)
	{
            print_error("error: cd: bad arguments\n");
            print_error(av[1]);
			print_error("\n");
	}
}

void	set_pipe(char last, int *pipefd, int fd)
{
//	printf("Vai setar o pipe se last = |\n");
	if (last == '|')
	{
//		printf("O ultimo é | \n");
		if (dup2(pipefd[fd], fd) == -1)
		{
			print_error("error: fatal\n");
			close(pipefd[0]);
			close(pipefd[1]);
			exit(1);
		}
//		write(2, "Passei do dup\n", 14);
		close(pipefd[0]);
		close(pipefd[1]);
	}
//	printf("Saindo de set_pipe\n");
}

void	print_error(char *str)
{
	int	i = 0;

	while (str[i])
	{
		write(2, &str, 1);
		i++;
	}
}

void	run_execve(char **av, char **env)
{
//	printf("Indo pro execve com av[0] = %s\n", av[0]);
	if (execve(av[0], av, env) == -1)
	{
			print_error("error: cannot execute \n");
	        print_error(av[0]);
	        print_error("\n");
	}
}
