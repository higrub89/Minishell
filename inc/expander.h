/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 10:57:31 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 10:59:28 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "minishell.h"

# define INITIAL_SB_CAPACITY 128

typedef struct s_string_builder
{
	char				*buffer;
	size_t				length;
	size_t				capacity;
}						t_string_builder;

typedef struct s_expansion_context
{
	t_string_builder	*sb;
	const char			*s;
	int					*i;
}						t_expansion_context;

// src/expander/expander_dollar.c
void					handle_special_dollar(t_expansion_context *ctx,
							t_struct *mini);
void					handle_alphanum_dollar(t_expansion_context *ctx,
							t_struct *mini);
void					process_dollar_expansion(t_expansion_context *ctx,
							t_struct *mini, char current_quote_char);
char					*extract_and_get_value(const char *s,
							int *start_of_var_idx, t_struct *mini);
void					init_expansion_ctx(t_expansion_context *ctx,
							t_string_builder *sb, const char *s, int *i);

// src/expander/expander_utils.c
char					**list_to_argv(t_list *lst);
void					free_argv(char **argv);
char					*expand_and_remove_quotes(char *original_str,
							t_struct *mini);
char					*expand_heredoc_line(char *line, t_struct *mini);
void					process_expansion_loop(t_expansion_context *ctx,
							t_struct *mini, char *original_str);

// src/expander/expander.c
void					expand_variables(t_command *cmd_list, t_struct *mini);
void					expand_command_args(t_command *cmd, t_struct *mini);
void					expand_command_redirections(t_command *cmd,
							t_struct *mini);
void					process_arg(char *arg, t_struct *mini,
							t_list **new_args_list);
void					split_and_add_args(char *expanded_str,
							t_list **args_list);

// src/expander/string_builder.c
void					ft_sb_init(t_string_builder *sb);
bool					ft_sb_grow(t_string_builder *sb, size_t needed);
void					ft_sb_append_char(t_string_builder *sb, char c);
void					ft_sb_append_str(t_string_builder *sb, const char *str);
char					*ft_sb_build(t_string_builder *sb);

#endif
