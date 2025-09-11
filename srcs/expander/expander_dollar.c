/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_dollar.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 22:53:34 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/04 00:12:13 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/expander.h"
#include "../../inc/builtins.h"

void	handle_special_dollar(t_expansion_context *ctx, t_struct *mini);
void	handle_alphanum_dollar(t_expansion_context *ctx, t_struct *mini);
void	process_dollar_expansion(t_expansion_context *ctx, t_struct *mini,
			char current_quote_char);
char	*extract_and_get_value(const char *s, int *start_of_var_idx,
			t_struct *mini);
void	init_expansion_ctx(t_expansion_context *ctx, t_string_builder *sb,
			const char *s, int *i);

void	handle_special_dollar(t_expansion_context *ctx, t_struct *mini)
{
	char	*var_value;

	if (ctx->s[*(ctx->i) + 1] == '?')
	{
		var_value = get_env_value("?", mini);
		if (var_value)
		{
			ft_sb_append_str(ctx->sb, var_value);
			free(var_value);
		}
		*(ctx->i) += 2;
	}
	else if (ft_isdigit(ctx->s[*(ctx->i) + 1]))
		*(ctx->i) += 2;
	else
	{
		ft_sb_append_char(ctx->sb, ctx->s[*(ctx->i)]);
	}
}

void	handle_alphanum_dollar(t_expansion_context *ctx, t_struct *mini)
{
	char	*var_value;
	int		start_of_var;

	start_of_var = *(ctx->i) + 1;
	var_value = extract_and_get_value(ctx->s, &start_of_var, mini);
	if (var_value)
	{
		ft_sb_append_str(ctx->sb, var_value);
		free(var_value);
	}
	*(ctx->i) = start_of_var;
}

void	process_dollar_expansion(t_expansion_context *ctx, t_struct *mini,
		char current_quote_char)
{
	if (current_quote_char == '\'')
	{
		ft_sb_append_char(ctx->sb, ctx->s[*(ctx->i)]);
		(*(ctx->i))++;
		return ;
	}
	if (ctx->s[*(ctx->i) + 1] == '\'' || ctx->s[*(ctx->i) + 1] == '"')
	{
		(*(ctx->i))++;
		return ;
	}
	if (ctx->s[*(ctx->i) + 1] == '?' || ft_isdigit(ctx->s[*(ctx->i) + 1]))
		handle_special_dollar(ctx, mini);
	else if (ft_isalpha(ctx->s[*(ctx->i) + 1]) || ctx->s[*(ctx->i) + 1] == '_')
		handle_alphanum_dollar(ctx, mini);
	else
	{
		ft_sb_append_char(ctx->sb, ctx->s[*(ctx->i)]);
		(*(ctx->i))++;
	}
}

char	*extract_and_get_value(const char *s, int *start_of_var_idx,
			t_struct *mini)
{
	int		temp_i;
	char	*var_name;
	char	*var_value;

	temp_i = *start_of_var_idx;
	while (s[temp_i] && (ft_isalnum(s[temp_i]) || s[temp_i] == '_'))
		temp_i++;
	var_name = ft_substr(s, *start_of_var_idx, temp_i - *start_of_var_idx);
	if (!var_name)
		return (NULL);
	var_value = get_env_value(var_name, mini);
	free(var_name);
	*start_of_var_idx = temp_i;
	return (var_value);
}

void	init_expansion_ctx(t_expansion_context *ctx, t_string_builder *sb,
			const char *s, int *i)
{
	ctx->sb = sb;
	ctx->s = s;
	ctx->i = i;
}
