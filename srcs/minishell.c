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
			break ;
	}
}

int	main(int argc, char **argv, char **envp_main)
{
	t_struct	mini;

	(void)argc;
	(void)argv;
	if (init_minishell(&mini, envp_main) != 0)
		return (1);
	run_minishell_loop(&mini);
	cleanup_minishell(&mini);
	return (mini.last_exit_status);
}
