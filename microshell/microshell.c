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
	argv++; // Ignora o primeiro argumento (nome do programa).
	while (argv[i])
	{
		if (strcmp(argv[i], "\0") != 0 && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0) // Incrementa o índice até encontrar um separador
			i++;
		else if (strcmp(argv[i - 1], ";") == 0 && strcmp(argv[i], ";") == 0) // Ignora argumentos repetidos de ponto e vírgula.
		{
			argv++;
			i = 0;
		}
		else if (i) // Encontrou um separador
		{
			execution(argv, i, envp); // Executa o comando armazenado em argv.
			argv += i + 1; // Avança para o próximo comando.
			i = 0;
		}
		else
			i++;
	}
	if (i) // Encerrou o argv com algum comando ainda por rodar
		execution(argv, i - 1, envp); // Executa o último comando se ainda houver algum.
}

/**
 * execution - Executes a command with support for pipes and the "cd" command.
 * @av: Array of command arguments.
 * @i: Index of the command delimiter.
 * @env: Environment variables array.
 *
 * This function checks the command to determine if it should be executed
 * directly (such as "cd") or forked into a new process with pipe support.
 * It handles setting up pipes, forking a child process, and executing the command
 * using run_execve.
 */

void	execution(char **av, int i, char **env)
{
	int pipefd[2];
	pid_t cpid;
	int *wstatus = NULL;

	if (strcmp(av[i], "|") != 0 && strcmp(av[0], "cd") == 0) // Verifica se o comando é 'cd' e o executa diretamente se não tiver pipes.
	{
		if (strcmp(av[i], ";") == 0) // O comando cd está seguido de um ";"
			do_cd(av, i - 1, 1); // Vai executar cd, retirando o índice do separador (;) e passando a flag de que há um separador ao final
		else // O comando cd não está seguido de um ";"
			do_cd(av, i, 2); // Vai executar cd, passando a flag de que não há nenhum separador ao final
		return ;
	}
	else if (av[i][0] == '|')
	{
		if (pipe(pipefd) < 0) // Faz o pipe. Se der erro, entra na condição e encerra o programa
		{
			print_error("error: fatal\n");
			exit(1);
		}
	}
	cpid = fork(); // Cria um processo filho para executar o comando.
	if (cpid < 0) // Caso o fork dê erro
	{
		print_error("error: fatal\n");
		exit(1);
	}
	if (cpid == 0) // No processo filho
	{
		set_pipe(av[i][0], pipefd, 1); // Configura o pipe para a saída. Ou seja, seta o fd_out do comando atual.
		if (av[i][0] == '|' || av[i][0] == ';')
			av[i] = NULL; // Substitui o separador por um "final de matriz falso". Dessa forma, não é necessário criar uma nova matriz para execve
		run_execve(av, env); // Executa o comando.
	}
	else if (cpid > 0)
		waitpid(cpid, wstatus, 0); // Processo pai espera o filho terminar.
	set_pipe(av[i][0], pipefd, 0); // Configura o pipe para a entrada do próximo comando. Ou seja, seta o fd_in já para o comando seguinte.
	return ;
}

/**
 * do_cd - Changes the current working directory.
 * @av: Array of command arguments.
 * @j: Index of the "cd" command.
 * @flag: Flag indicating if the "cd" command is followed by ";"
 *
 * This function changes the current working directory to the specified
 * path provided in av[1]. It checks if the command has valid arguments
 * and displays an error if the directory change fails.
 */

void	do_cd(char **av, int j, int flag)
{
	if (j != 1 && flag == 1) // Verifica se cd não tem exatamente um complemento e se tem um ";" depois desse comando
	{
		print_error("error: cd: bad arguments\n");
		return ;
	}
	else if (j != 1 && flag == 2) // Verifica se cd não tem exatamente um complemento e se não há mais nada depois desse comando, no input
		return ;
	if (chdir(av[1]) == -1) // Tenta mudar para o diretório especificado.
	{
            print_error("error: cd: bad arguments\n");
            print_error(av[1]);
			print_error("\n");
	}
}

/**
 * set_pipe - Configures the pipe for inter-process communication.
 * @last: Character indicating the presence of a pipe ('|').
 * @pipefd: File descriptors array for the pipe.
 * @fd: File descriptor to redirect (0 for input, 1 for output).
 *
 * This function duplicates the pipe file descriptor to redirect input or
 * output for the child process based on the command's requirements.
 * It closes both ends of the pipe after duplication to avoid resource leaks.
 */

void	set_pipe(char last, int *pipefd, int fd)
{
	if (last == '|') // Altera o FD apenas se o comando tiver um pipe no final
	{
		if (dup2(pipefd[fd], fd) == -1)
		{
			print_error("error: fatal\n");
			close(pipefd[0]);
			close(pipefd[1]);
			exit(1);
		}
		close(pipefd[0]); // Fecha o pipe de entrada
		close(pipefd[1]); // Fecha o pipe de saída
	}
}

/**
 * print_error - Prints an error message to standard error.
 * @str: The error message to print.
 *
 * This function writes each character of the error message string to
 * the standard error (stderr) file descriptor. It’s used to notify the
 * user of fatal errors or failed command executions.
 */

void	print_error(char *str)
{
	int	i = 0;

	while (str[i])
	{
		write(2, &str, 1); // Escreve cada caractere da string no stderr.
		i++;
	}
}

/**
 * run_execve - Executes a command using execve.
 * @av: Array of command arguments.
 * @env: Environment variables array.
 *
 * This function replaces the current process image with a new process
 * specified by av[0], using the environment variables from env. If execve fails,
 * it displays an error message and returns control to the calling process.
 */

void	run_execve(char **av, char **env)
{
	if (execve(av[0], av, env) == -1) // Tenta executar o comando.
	{
			print_error("error: cannot execute \n");
	        print_error(av[0]);
	        print_error("\n");
	}
}
