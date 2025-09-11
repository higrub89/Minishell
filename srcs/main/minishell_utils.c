/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 21:58:44 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/10 21:58:46 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"
#include "../inc/lexer.h"
#include "../inc/main_utils.h"
#include "../inc/minishell.h"
#include "../inc/parser.h"

static char	*get_input(void)
{
	char	*input_line;

	set_signals(INTERACTIVE);
	input_line = readline("minishell> ");
	if (!input_line)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		return (NULL);
	}
	if (ft_strlen(input_line) > 0 && !ft_str_is_whitespace(input_line))
		add_history(input_line);
	return (input_line);
}

static void	process_and_execute(char *input_line, t_struct *mini)
{
	t_token		*token_list;
	t_command	*commands;

	token_list = NULL;
	commands = NULL;
	if (process_input(input_line, &token_list, &commands, mini))
	{
		expand_variables(commands, mini);
		execute_commands(commands, mini);
	}
	free_commands(commands);
}

void	run_minishell_loop(t_struct *mini)
{
	char	*input_line;

	while (1)
	{
		input_line = get_input();
		if (!input_line)
			break ;
		process_and_execute(input_line, mini);
		set_signals(INTERACTIVE);
		if (mini->last_exit_status == 130 || mini->child_interrupted_by_signal)
		{
			rl_on_new_line();
			rl_replace_line("", 0);
			mini->child_interrupted_by_signal = false;
		}
		free(input_line);
		if (mini->should_exit)
			clean_and_exit(mini, mini->last_exit_status);
	}
}
