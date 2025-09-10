/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 22:55:19 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 22:55:20 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/expander.h"

char	**list_to_argv(t_list *lst);
void	free_argv(char **argv);
char	*expand_and_remove_quotes(char *original_str, t_struct *mini);
char	*expand_heredoc_line(char *line, t_struct *mini);
void	process_expansion_loop(t_expansion_context *ctx, t_struct *mini,
			char *original_str);

char	**list_to_argv(t_list *lst)
{
	char	**argv;
	int		size;
	int		i;

	size = ft_lstsize(lst);
	argv = (char **)ft_calloc(size + 1, sizeof(char *));
	if (!argv)
		return (NULL);
	i = 0;
	while (lst)
	{
		argv[i] = (char *)lst->content;
		lst = lst->next;
		i++;
	}
	return (argv);
}

void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i++]);
	}
	free(argv);
}

char	*expand_and_remove_quotes(char *original_str, t_struct *mini)
{
	t_string_builder	sb;
	t_expansion_context	ctx;
	int					i;

	if (!original_str)
		return (ft_strdup(""));
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL);
	i = 0;
	init_expansion_ctx(&ctx, &sb, original_str, &i);
	process_expansion_loop(&ctx, mini, original_str);
	return (ft_sb_build(&sb));
}

char	*expand_heredoc_line(char *line, t_struct *mini)
{
	t_string_builder	sb;
	t_expansion_context	ctx;
	int					i;

	if (!line)
		return (NULL);
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL);
	i = 0;
	ctx.sb = &sb;
	ctx.s = line;
	ctx.i = &i;
	while (line[i])
	{
		if (line[i] == '$')
			process_dollar_expansion(&ctx, mini, 0);
		else
			ft_sb_append_char(&sb, line[i++]);
	}
	return (ft_sb_build(&sb));
}

void	process_expansion_loop(t_expansion_context *ctx, t_struct *mini,
		char *original_str)
{
	char	current_quote_char;

	current_quote_char = 0;
	while (original_str[*(ctx->i)])
	{
		if (original_str[*(ctx->i)] == '\'' || original_str[*(ctx->i)] == '"')
		{
			if (current_quote_char == 0)
				current_quote_char = original_str[*(ctx->i)];
			else if (current_quote_char == original_str[*(ctx->i)])
				current_quote_char = 0;
			else
				ft_sb_append_char(ctx->sb, original_str[*(ctx->i)]);
			(*(ctx->i))++;
			continue ;
		}
		if (original_str[*(ctx->i)] == '$')
			process_dollar_expansion(ctx, mini, current_quote_char);
		else
			ft_sb_append_char(ctx->sb, original_str[(*(ctx->i))++]);
	}
}
