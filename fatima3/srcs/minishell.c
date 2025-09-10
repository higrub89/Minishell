/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:41:19 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/04 00:11:25 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/main_utils.h"
#include "../inc/executor.h"

int	process_input(char *input, t_token **tokens, t_command **cmds,
	t_struct *mini)
{
	*tokens = lexer(input, mini);
	if (!*tokens)
	{
		free(input);
		return (0);
	}
	*cmds = parse_input(*tokens, mini);
	free_tokens(*tokens);
	if (!*cmds)
	{
		free(input);
		return (0);
	}
	return (1);
}

void	run_minishell_loop(t_struct *mini)
{
	char		*input_line;
	t_token		*token_list;
	t_command	*commands;

	while (1)
	{
		token_list = NULL;
		commands = NULL;
		set_signals(INTERACTIVE);
		input_line = readline("minishell> ");
		if (!input_line)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (ft_strlen(input_line) > 0 && !ft_str_is_whitespace(input_line))
			add_history(input_line);
		if (!process_input(input_line, &token_list, &commands, mini))
			continue ;
		expand_variables(commands, mini);
		execute_commands(commands, mini);
		free_commands(commands);
		free(input_line);
		if (mini->should_exit)
			clean_and_exit(mini, mini->last_exit_status);
	}
}

static int	handle_empty_env(t_struct *mini, char *arg) //agregada toda esta fx
{
	char	*cwd;
	char	*tmp;
	char	*args[3];

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: cd: getcwd failed");
		return (mini->last_exit_status = 1, 1);
	}
	tmp = ft_strjoin("PWD=", cwd);
	free(cwd);
	if (!tmp)
		return (mini->last_exit_status = 1, 1);
	args[0] = "export";
	args[1] = tmp;
	args[2] = NULL;
	ft_export(mini, args);
	free(tmp);
	manage_shlvl(mini);
	handle_underscore(mini, arg);
	return (0);
}

int	init_minishell(t_struct *mini, char **envp_main, char *arg) //lo moví aquí
{
	mini->envp = ft_copy_str_array(envp_main);
	if (!mini->envp)
	{
		perror("minishell: failed to copy environment");
		return (1);
	}
	if (!mini->envp[0] && handle_empty_env(mini, arg)) //agregado
        return (1); //agregado
	manage_shlvl(mini);
	mini->last_exit_status = 0;
	mini->export_list = NULL;
	mini->should_exit = false;
	return (0);
}

int	main(int argc, char **argv, char **envp_main)
{
	t_struct	mini;

	(void)argc;
	(void)argv;
	if (init_minishell(&mini, envp_main, argv[0]) != 0) // argv[0] agregado para saber la ruta al ejecutable de la mini
		return (1);
	run_minishell_loop(&mini);
	cleanup_minishell(&mini);
	return (mini.last_exit_status);
}
