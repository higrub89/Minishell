/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:03:51 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/11 23:31:04 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

static int	handle_initial_checks(t_command *commands, t_struct *mini)
{
	set_signals(NON_INTERACTIVE);
	mini->is_piped = false;
	if (process_heredocs(commands, mini) != 0)
		return (1);
	if ((!commands->args || !commands->args[0]) && commands->redirections
		&& !commands->next)
		return (handle_single_redirection_only(commands, mini));
	if ((!commands->args || !commands->args[0]) && !commands->redirections)
		return (0);
	return (-1);
}

static int	execute_single_command(t_command *commands, t_struct *mini)
{
	if (commands->args && commands->args[0] && is_builtin(commands->args[0]))
		return (handle_single_builtin(commands, mini));
	return (-1);
}

static int	execute_pipeline_commands(t_command *commands, t_struct *mini,
		pid_t *child_pids, int num_commands)
{
	int	result;

	result = execute_pipeline(commands, mini, child_pids, num_commands);
	return (result);
}

int	execute_commands(t_command *commands, t_struct *mini)
{
	pid_t	*child_pids;
	int		num_commands;
	int		result;

	result = handle_initial_checks(commands, mini);
	if (result != -1)
	{
		set_signals(INTERACTIVE);
		return (result);
	}
	num_commands = init_pipeline(commands, &child_pids);
	if (num_commands == -1)
		result = (mini->last_exit_status = 1, 1);
	else
	{
		result = execute_single_command(commands, mini);
		if (result == -1)
			result = execute_pipeline_commands(commands, mini, child_pids,
					num_commands);
	}
	set_signals(INTERACTIVE);
	free(child_pids);
	return (result);
}
