/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 02:05:31 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 02:05:34 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/parser.h"

t_command	*create_command_node(void)
{
	t_command	*new_cmd;

	new_cmd = (t_command *)ft_calloc(1, sizeof(t_command));
	if (!new_cmd)
	{
		perror("minishell: malloc failed in create_command_node");
		return (NULL);
	}
	new_cmd->heredoc_fd = -1;
	return (new_cmd);
}

char	*remove_quotes(char *str)
{
	char	*new_str;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len > 1 && ((str[0] == '\'' && str[len - 1] == '\'') || (str[0] == '\"'
				&& str[len - 1] == '\"')))
	{
		new_str = ft_substr(str, 1, len - 2);
		free(str);
		return (new_str);
	}
	return (str);
}

t_redirection	*create_redirection_node(t_redir_type type, char *file)
{
	t_redirection	*new_redir;
	char			*file_content;

	new_redir = (t_redirection *)ft_calloc(1, sizeof(t_redirection));
	if (!new_redir)
		return (perror("minishell: malloc failed"), NULL);
	file_content = ft_strdup(file);
	if (!file_content)
	{
		perror("minishell: ft_strdup failed");
		free(new_redir);
		return (NULL);
	}
	new_redir->type = type;
	new_redir->expand_heredoc_content = (type == REDIR_HEREDOC
			&& !ft_strchr(file, '\'') && !ft_strchr(file, '\"'));
	if (type == REDIR_HEREDOC)
		new_redir->file = remove_quotes(file_content);
	else
		new_redir->file = file_content;
	return (new_redir);
}

void	add_redir_to_command(t_command *cmd, t_redirection *new_redir)
{
	t_redirection	*last_redir;

	if (!cmd || !new_redir)
		return ;
	if (!cmd->redirections)
		cmd->redirections = new_redir;
	else
	{
		last_redir = cmd->redirections;
		while (last_redir->next)
			last_redir = last_redir->next;
		last_redir->next = new_redir;
	}
	cmd->num_redirections++;
}

int	add_arg_to_command(t_command *cmd, const char *arg)
{
	char	**new_args;
	int		i;

	if (!cmd || !arg)
		return (0);
	new_args = (char **)ft_calloc(cmd->num_args + 2, sizeof(char *));
	if (!new_args)
		return (perror("minishell: malloc failed"), 0);
	i = -1;
	while (++i < cmd->num_args)
		new_args[i] = cmd->args[i];
	new_args[i] = ft_strdup(arg);
	if (!new_args[i])
	{
		perror("minishell: ft_strdup failed");
		free(new_args);
		return (0);
	}
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->num_args++;
	return (1);
}
