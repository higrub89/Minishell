/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 21:48:52 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/10 21:48:53 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

static void	init_exec_data(t_exec_data *d, pid_t *child_pids, t_struct *mini)
{
	d->prev_fd = -1;
	d->cmd_idx = 0;
	d->child_pids = child_pids;
	d->mini = mini;
}

static int	handle_command(t_command *curr, t_exec_data *d)
{
	d->pid = create_pipe_and_fork(curr, d->pipe_fd);
	if (d->pid == -1)
	{
		handle_fork_error(curr, d);
		return (1);
	}
	if (d->pid == 0)
		child_process(curr, d->pipe_fd, d->prev_fd, d->mini);
	else
		parent_process(curr, d);
	return (0);
}

int	execute_pipeline(t_command *cmds, t_struct *mini, pid_t *child_pids,
		int num_commands)
{
	t_command	*curr;
	t_exec_data	d;

	set_signals(NON_INTERACTIVE);
	init_exec_data(&d, child_pids, mini);
	curr = cmds;
	while (curr)
	{
		if (handle_command(curr, &d))
			return (1);
		curr = curr->next;
	}
	wait_for_children(cmds, child_pids, num_commands, mini);
	set_signals(INTERACTIVE);
	return (0);
}
