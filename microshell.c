
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int write_error(char *str, char *argv)
{
	int i = -1;
	
	while (str[++i])
		write(2, &str[i], 1);
	i = -1;
	if (argv)
		while (argv[++i])
			write(2, &argv[i], 1);
	write(2, "\n", 1);
	return (1);
}

int ft_exe(char **argv, int i, int fd_in, char **env)
{
	argv[i] = NULL;
	dup2(fd_in, STDIN_FILENO);
	close(fd_in);
	execve(argv[0], argv, env);
	return (write_error("error: cannot execute ", argv[0]));
}

void execute_commands(char **argv, int fd_in, char **env)
{
	int i = 0;
	int fd[2];

	while(argv[i] && argv[i + 1])
	{
		argv = &(argv[i + 1]);
		i = 0;
		while(argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			++i;
		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				write_error("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]) != 0)
				write_error("error: error: cd: cannot change directory to ", argv[1]);
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
		{
			if (fork() == 0)
			{
				if (ft_exe(argv, i, fd_in, env))
					_exit(1);
			}
			else
			{
				close(fd_in);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					fd_in = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && !strcmp(argv[i], "|"))
		{
			pipe(fd);
			if (fork() == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				if (ft_exe(argv, i, fd_in, env))
					_exit(1);
			}
			else
			{
				close(fd[1]);
				close(fd_in);
				fd_in = fd[0];
			}
		}
	}
	close(fd_in);
}

int main(int argc, char **argv, char **env)
{
	(void) argc;
	int fd_in = dup(STDIN_FILENO);
	execute_commands(argv, fd_in, env);
	return (0);
}