#include "../inc/minishell.h"

int	g_last_exit_status = 0;

int	main(int argc, char **argv, char **envp)
{
	char		*input_line;
	t_token		*token_list;
	t_command	*commands;

	(void)argc;
	(void)argv;
	while (1)
	{
		input_line = readline("minishell> ");
		if (!input_line) // CTRL + D
		{
			printf("exit\n");
			break ;
		}
		if (ft_strlen(input_line) > 0)
			add_history(input_line);
		token_list = lexer(input_line);
		if (!token_list)
		{
			free(input_line);
			continue ;
		}
		commands = parse_input(token_list, envp);
		if (!commands)
		{
			free(input_line);
			continue ;
		}
		expand_variables(commands, envp, &g_last_exit_status);
		if (commands)
		{
			execute_commands(commands, envp, &g_last_exit_status);
		}
		else
		{
			g_last_exit_status = 0;
		}
		free_commands(commands);
		free(input_line);
	}
	return (g_last_exit_status);
}
