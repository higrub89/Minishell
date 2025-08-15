/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* expander.c                                         :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/* Updated: 2025/06/24 16:34:10 by rhiguita         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "../inc/expander.h"

// --- Funciones de String Builder ---

// Inicializa el string builder
static void	ft_sb_init(t_string_builder *sb)
{
	sb->length = 0;
	sb->capacity = INITIAL_SB_CAPACITY;
	sb->buffer = (char *)ft_calloc(sb->capacity, sizeof(char));
	if (!sb->buffer)
	{
		sb->capacity = 0;
	}
}

// Asegura que hay suficiente espacio en el buffer.
static bool	ft_sb_grow(t_string_builder *sb, size_t needed)
{
	char	*new_buffer;
	size_t	new_capacity;

	if (!sb->buffer) // Si el buffer inicializó con error
		return (false);
	if (sb->length + needed < sb->capacity) // Ya hay suficiente espacio
		return (true);
	new_capacity = sb->capacity;
	while (sb->length + needed >= new_capacity)
		// Duplicar capacidad hasta que sea suficiente
		new_capacity *= 2;
	new_buffer = (char *)ft_calloc(new_capacity, sizeof(char));
	if (!new_buffer) // Error de malloc
	{
		free(sb->buffer);
		sb->buffer = NULL;
		sb->capacity = 0;
		sb->length = 0;
		return (false);
	}
	ft_memcpy(new_buffer, sb->buffer, sb->length); // Copiar contenido existente
	free(sb->buffer);                              // Liberar buffer antiguo
	sb->buffer = new_buffer;
	sb->capacity = new_capacity;
	return (true);
}

// Añade un caracter al string builder
static void	ft_sb_append_char(t_string_builder *sb, char c)
{
	if (!ft_sb_grow(sb, 1))
		return ; // Manejar error de crecimiento (ej. setear g_last_exit_status)
	sb->buffer[sb->length] = c;
	sb->length++;
	sb->buffer[sb->length] = '\0'; // Mantener null-terminated
}

// Añade una cadena al string builder
static void	ft_sb_append_str(t_string_builder *sb, const char *str)
{
	size_t	str_len;

	if (!str || !sb->buffer)
		return ; // Manejar error si str es NULL o sb->buffer es NULL
	str_len = ft_strlen(str);
	if (!ft_sb_grow(sb, str_len))
		return ; // Manejar error de crecimiento
	ft_memcpy(sb->buffer + sb->length, str, str_len);
	sb->length += str_len;
	sb->buffer[sb->length] = '\0'; // Mantener null-terminated
}

// Finaliza y retorna la cadena construida, liberando los recursos del builder.
static char	*ft_sb_build(t_string_builder *sb)
{
	char	*final_str;

	if (!sb->buffer)
		return (NULL);                
			// Si hubo un error en la inicialización o crecimiento
	final_str = ft_strdup(sb->buffer); // Duplicar la cadena final
	free(sb->buffer);                  // Liberar el buffer interno del builder
	sb->buffer = NULL;
	sb->length = 0;
	sb->capacity = 0;
	return (final_str);
}

// --- Funciones de Expansión ---

static char	*extract_and_get_value(const char *s, int *start_of_var_idx,
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
		return (NULL); // Malloc error
	var_value = get_env_value(var_name, mini);
	free(var_name);             // Free the extracted variable name
	*start_of_var_idx = temp_i; // Update index for main loop
	return (var_value);
}

// Handles special dollar expansions like $? or $0.
static void	handle_special_dollar(t_string_builder *sb, const char *s, int *i,
		t_struct *mini)
{
	char	*var_value;

	if (s[*i + 1] == '?')
	{
		var_value = get_env_value("?", mini);
		if (var_value)
		{
			ft_sb_append_str(sb, var_value);
			free(var_value);
		}
		(*i) += 2; // Move past $?
	}
	else if (ft_isdigit(s[*i + 1])) // $0, $1, etc. (positional parameters)
	{
		ft_sb_append_char(sb, '$');
		ft_sb_append_char(sb, s[*i + 1]);
		(*i) += 2; // Move past $ and digit
	}
	else // Literal '$' (e.g., $ followed by non-var char, or just $)
	{
		ft_sb_append_char(sb, s[*i]); // Append the '$' literally
		(*i)++;
	}
}

// Handles alphanumeric dollar expansions ($VAR).
static void	handle_alphanum_dollar(t_string_builder *sb, const char *s, int *i,
		t_struct *mini)
{
	char	*var_value;
	int		start_of_var;

	start_of_var = *i + 1; // Position after '$'
	var_value = extract_and_get_value(s, &start_of_var, mini);
	if (var_value) // If value was found (even if empty string)
	{
		ft_sb_append_str(sb, var_value);
		free(var_value);
	}
	*i = start_of_var; // Update main iterator
}

// Orchestrates dollar sign expansion.
static void	process_dollar_expansion(t_string_builder *sb, const char *s,
		int *i, t_struct *mini, char current_quote_char)
{
	if (current_quote_char == '\'') // Inside single quotes, no expansion
	{
		ft_sb_append_char(sb, s[*i]); // Append '$' literally
		(*i)++;
		return ;
	}
	// Check for potential variable after '$'
	if (s[*i + 1] == '?' || ft_isdigit(s[*i + 1]))
		handle_special_dollar(sb, s, i, mini);
	else if (ft_isalpha(s[*i + 1]) || s[*i + 1] == '_')
		handle_alphanum_dollar(sb, s, i, mini);
	else // '$' followed by whitespace, null terminator, or other non-var char
	{
		ft_sb_append_char(sb, s[*i]); // Append '$' literally
		(*i)++;
	}
}

// --- Main Expansion Logic ---

// Expands variables and removes all quotes from a string.
// This function receives the string AS-IS from the parser (with quotes).
char	*expand_string(char *original_str, t_struct *mini)
{
	t_string_builder	sb;
	int					i;

	char current_quote_char; // 0 for no quote, '\'' or '\"'
	if (!original_str)
		return (ft_strdup("")); // Return empty string for NULL input
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL); // Malloc failure
	i = 0;
	current_quote_char = 0;
	while (original_str[i])
	{
		if (original_str[i] == '\'' || original_str[i] == '\"')
		{
			if (current_quote_char == original_str[i]) // Closing quote
				current_quote_char = 0;
			else if (current_quote_char == 0) // Opening quote
				current_quote_char = original_str[i];
			i++; // Skip the quote char itself
			continue ;
		}
		if (original_str[i] == '$' && original_str[i + 1])
			process_dollar_expansion(&sb, original_str, &i, mini,
				current_quote_char);
		else // Regular character, or '$' at end of string
		{
			ft_sb_append_char(&sb, original_str[i]);
			i++;
		}
	}
	return (ft_sb_build(&sb));
}

// Iterates through all commands and expands their arguments and redirections.
void	expand_variables(t_command *cmd_list, t_struct *mini)
{
	t_command		*current_cmd;
	t_redirection	*current_redir;
	char			*expanded_str;
	int				i;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		i = 0;
		while (current_cmd->args && current_cmd->args[i])
		{
			expanded_str = expand_string(current_cmd->args[i], mini);
			if (!expanded_str) // Malloc error
			{
				mini->last_exit_status = 1; // Indicate memory error
				return ;                    
			}
			free(current_cmd->args[i]);
			current_cmd->args[i] = expanded_str;
			i++;
		}
		current_redir = current_cmd->redirections;
		while (current_redir)
		{
			if (current_redir->type == REDIR_HEREDOC && !current_redir->expand_heredoc_content)
			{
				expanded_str = expand_string(current_redir->file, mini);
				if (!expanded_str)
				{
					mini->last_exit_status = 1;
					return ;
				}
				free(current_redir->file);
				current_redir->file = expanded_str;
			}
			else if (current_redir->type != REDIR_HEREDOC)
			{
				expanded_str = expand_string(current_cmd->args[i], mini);
				if (!expanded_str)
				{
					mini->last_exit_status = 1;
					return ;
				}
				free(current_redir->file);
				current_redir->file = expanded_str;
			}
			current_redir = current_redir->next;
		}
		current_cmd = current_cmd->next;
	}
}