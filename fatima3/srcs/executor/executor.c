/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:03:51 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:04:04 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

pid_t	create_pipe_and_fork(t_command *cmd, int pipe_fd[2])
{
	if (cmd->next && pipe(pipe_fd) == -1)
		return (perror("pipe"), -1);
	return (fork());
}

void	child_process(t_command *cmd, int pipe_fd[2], int prev_fd,
		t_struct *mini)
{
	mini->is_piped = true;
	set_signals(CHILD);
	setup_child_pipes(cmd, pipe_fd, prev_fd);
	child_execute_command(cmd, mini);
}

void	parent_process(t_command *cmd, t_exec_data *d)
{
	parent_cleanup(cmd, d);
	d->cmd_idx++;
}

int	execute_pipeline(t_command *cmds, t_struct *mini, pid_t *child_pids,
		int num_commands)
{
	t_command	*curr;
	t_exec_data	d;

	set_signals(NON_INTERACTIVE);
	curr = cmds;
	d.prev_fd = -1;
	d.cmd_idx = 0;
	d.child_pids = child_pids;
	d.mini = mini;
	while (curr)
	{
		d.pid = create_pipe_and_fork(curr, d.pipe_fd);
		if (d.pid == -1)
		{
			handle_fork_error(curr, &d);
			return (1);
		}
		if (d.pid == 0)
			child_process(curr, d.pipe_fd, d.prev_fd, mini);
		else
			parent_process(curr, &d);
		curr = curr->next;
	}
	wait_for_children(cmds, child_pids, num_commands, mini);
	set_signals(INTERACTIVE);
	return (0);
}

int	execute_commands(t_command *commands, t_struct *mini)
{
	pid_t	*child_pids;
	int		num_commands;
	int		result;

	mini->is_piped = false;
	if (process_heredocs(commands, mini) != 0)
		return (1);
	if ((!commands->args || !commands->args[0]) && commands->redirections
		&& !commands->next)
		return (handle_single_redirection_only(commands, mini));
	if ((!commands->args || !commands->args[0]) && !commands->redirections)
		return (0);
	num_commands = init_pipeline(commands, &child_pids);
	if (num_commands == -1)
		return (mini->last_exit_status = 1, 1);
	if (num_commands == 1 && commands->args && commands->args[0]
		&& is_builtin(commands->args[0]))
		result = handle_single_builtin(commands, mini);
	else
		result = execute_pipeline(commands, mini, child_pids, num_commands);
	free(child_pids);
	return (result);
}
