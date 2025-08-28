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

// --- Funciones de Expansión ---

/**
 * @brief Expande variables en una cadena y ELIMINA las comillas.
 * Usado para argumentos de comandos y nombres de archivo de redirección.
 */
char	*expand_and_remove_quotes(char *original_str, t_struct *mini)
{
	t_string_builder	sb;
	int					i;
	char				current_quote_char;

	if (!original_str)
		return (ft_strdup(""));
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL);
	i = 0;
	current_quote_char = 0;
	while (original_str[i])
	{
		if (original_str[i] == '\'' || original_str[i] == '\"')
		{
			if (current_quote_char == 0)
				current_quote_char = original_str[i];
			else if (current_quote_char == original_str[i])
				current_quote_char = 0;
			i++; // Saltar la comilla, no la añadimos al resultado
			continue ;
		}
		if (original_str[i] == '$')
			process_dollar_expansion(&sb, original_str, &i, mini, current_quote_char);
		else
			ft_sb_append_char(&sb, original_str[i++]);
	}
	return (ft_sb_build(&sb));
}

/**
 * @brief Expande variables en una línea de heredoc. NO elimina comillas.
 */
char	*expand_heredoc_line(char *line, t_struct *mini)
{
	t_string_builder	sb;
	int					i;

	if (!line)
		return (NULL);
	ft_sb_init(&sb);
	if (!sb.buffer)
		return (NULL);
	i = 0;
	while (line[i])
	{
		// En heredoc, solo expandimos si el delimitador no tenía comillas.
		// Las comillas dentro de la línea son literales.
		if (line[i] == '$')
			process_dollar_expansion(&sb, line, &i, mini, 0); // 0 = sin comillas
		else
			ft_sb_append_char(&sb, line[i++]);
	}
	return (ft_sb_build(&sb));
}

/**
 * @brief Bucle principal de expansión. Procesa argumentos y delimitadores de heredoc.
 */
void	expand_variables(t_command *cmd_list, t_struct *mini)
{
	t_command		*cmd;
	t_redirection	*redir;
	char			*expanded_str;
	int				i;

	cmd = cmd_list;
	while (cmd)
	{
		i = 0;
		while (cmd->args && cmd->args[i])
		{
			expanded_str = expand_and_remove_quotes(cmd->args[i], mini);
			free(cmd->args[i]);
			cmd->args[i++] = expanded_str;
		}
		redir = cmd->redirections;
		while (redir)
		{
			// El delimitador de Heredoc SIEMPRE se expande/limpia de comillas.
			// La expansión del CONTENIDO se decide en el parser.
			if (redir->type == REDIR_HEREDOC)
				expanded_str = expand_and_remove_quotes(redir->file, mini);
			else // Los nombres de archivo también se expanden
				expanded_str = expand_and_remove_quotes(redir->file, mini);
			free(redir->file);
			redir->file = expanded_str;
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}