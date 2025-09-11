/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 22:53:27 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/expander.h"

void	expand_variables(t_command *cmd_list, t_struct *mini);
void	expand_command_args(t_command *cmd, t_struct *mini);
void	expand_command_redirections(t_command *cmd, t_struct *mini);
void	process_arg(char *arg, t_struct *mini, t_list **new_args_list);
void	split_and_add_args(char *expanded_str, t_list **args_list);

void	process_arg(char *arg, t_struct *mini, t_list **new_args_list)
{
	char	*expanded_str;
	bool	was_quoted;

	was_quoted = ft_strchr(arg, '\'') || ft_strchr(arg, '"');
	expanded_str = expand_and_remove_quotes(arg, mini);
	if (!expanded_str)
		expanded_str = ft_strdup("");
	if (!expanded_str)
		return ;
	if (!was_quoted && ft_strchr(expanded_str, ' '))
		split_and_add_args(expanded_str, new_args_list);
	else
	{
		if (was_quoted || ft_strlen(expanded_str) > 0)
			ft_lstadd_back(new_args_list, ft_lstnew(expanded_str));
		else
			free(expanded_str);
	}
}

void	expand_command_args(t_command *cmd, t_struct *mini)
{
	t_list	*new_args_list;
	int		i;

	new_args_list = NULL;
	i = -1;
	while (cmd->args[++i])
		process_arg(cmd->args[i], mini, &new_args_list);
	free_argv(cmd->args);
	cmd->args = list_to_argv(new_args_list);
	if (!cmd->args)
		ft_lstclear(&new_args_list, free);
	else
		ft_lstclear(&new_args_list, NULL);
	cmd->num_args = ft_lstsize(new_args_list);
}

void	split_and_add_args(char *expanded_str, t_list **args_list)
{
	char	**split_args;
	int		j;

	split_args = ft_split(expanded_str, ' ');
	free(expanded_str);
	if (!split_args)
		return ;
	j = -1;
	while (split_args[++j])
		ft_lstadd_back(args_list, ft_lstnew(split_args[j]));
	free(split_args);
}

void	expand_command_redirections(t_command *cmd, t_struct *mini)
{
	t_redirection	*redir;
	char			*expanded_str;

	redir = cmd->redirections;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
		{
		}
		else
		{
			expanded_str = expand_and_remove_quotes(redir->file, mini);
			if (expanded_str)
			{
				free(redir->file);
				redir->file = expanded_str;
			}
		}
		redir = redir->next;
	}
}

void	expand_variables(t_command *cmd_list, t_struct *mini)
{
	t_command	*cmd;

	cmd = cmd_list;
	while (cmd)
	{
		if (cmd->args)
			expand_command_args(cmd, mini);
		expand_command_redirections(cmd, mini);
		cmd = cmd->next;
	}
}
