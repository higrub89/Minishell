
#include "../inc/parser.h"

// Crea un nuevo comando.
t_command	*create_command_node(void)
{
	t_command	*new_cmd;

	new_cmd = (t_command *)ft_calloc(1, sizeof(t_command));
	if (!new_cmd)
	{
		perror("minishell: malloc failed in create_command_node");
		return (NULL);
	}
	new_cmd->args = NULL;
	new_cmd->num_args = 0;
	new_cmd->redirections = NULL;
	new_cmd->num_redirections = 0;
	new_cmd->full_path = NULL;
	new_cmd->heredoc_fd = -1;
	new_cmd->next = NULL;
	new_cmd->prev = NULL;
	return (new_cmd);
}

// Elimina las comillas de la cadena si existen
static char *remove_quotes(char *str)
{
    char *new_str;
    int len;

    if (!str)
        return NULL;
    len = ft_strlen(str);
    if ((str[0] == '\'' && str[len - 1] == '\'') ||
        (str[0] == '\"' && str[len - 1] == '\"'))
    {
        // Si hay comillas, crea una subcadena sin ellas.
        new_str = ft_substr(str, 1, len - 2);
        free(str);
        return new_str;
    }
    return str; // No hay comillas, devuelve la cadena original.
}

// Crea un nuevo nodo de redirección.
t_redirection	*create_redirection_node(t_redir_type type, char *file)
{
	t_redirection	*new_redir;
	char *stripped_file;

	new_redir = (t_redirection *)ft_calloc(1, sizeof(t_redirection));
	if (!new_redir)
	{
		perror("minishell: malloc failed in create_redirection_node");
		return (NULL);
	}
	if (type == REDIR_HEREDOC)
	{
		stripped_file = remove_quotes(ft_strdup(file));
	}
	else
	{
		stripped_file = ft_strdup(file);
	}
	if (!stripped_file)
	{
		perror("minishell: ft_strdup failed in create_redirection_node");
		free(new_redir);
		return (NULL);
	}
	new_redir->type = type;
	new_redir->file = stripped_file;
	new_redir->expand_heredoc_content = (type == REDIR_HEREDOC
			&& !ft_strchr(file, '\'') && !ft_strchr(file, '\"'));
	new_redir->next = NULL;
	return (new_redir);
}

// Añade una redirección a la lista de redirecciones de un comando.
void	add_redir_to_command(t_command *cmd, t_redirection *new_redir)
{
	t_redirection	*last_redir;

	if (!cmd || !new_redir)
		return ;
	if (!cmd->num_redirections)
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

// Auxiliar para add_arg_to_command: asigna memoria para el nuevo array de argumentos.
static char	**allocate_new_args(t_command *cmd)
{
	char	**new_args;
	int		i;

	new_args = (char **)ft_calloc(cmd->num_args + 2, sizeof(char *));
	if (!new_args)
	{
		perror("minishell: malloc failed for new_args");
		return (NULL);
	}
	i = -1;
	while (++i < cmd->num_args)
		new_args[i] = cmd->args[i];
	return (new_args);
}

// Maneja el error de strdup en add_arg_to_command.
static int	add_arg_strdup_error(char **new_args)
{
	perror("minishell: ft_strdup failed for new arg");
	free(new_args);
	return (0);
}

// Añade un argumento a la lista de argumentos de un comando.
int	add_arg_to_command(t_command *cmd, const char *arg)
{
	char	**new_args;

	if (!cmd || !arg)
		return (1);
	new_args = allocate_new_args(cmd);
	if (!new_args)
		return (0);
	new_args[cmd->num_args] = ft_strdup(arg);
	if (!new_args[cmd->num_args])
		return (add_arg_strdup_error(new_args));
	new_args[cmd->num_args + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->num_args++;
	return (1);
}

// Función para limpiar en caso de error de parsing.
static void	parser_error_cleanup(t_command *cmd_list_head,
		t_struct *mini)
{
	if (cmd_list_head)
		free_commands(cmd_list_head);
	mini->last_exit_status = 258;
}

// Auxiliar para process_redirection: obtiene el tipo de redirección.
static t_redir_type	get_redirection_type(t_token *token)
{
	if (ft_strcmp(token->value, "<") == 0)
		return (REDIR_IN);
	else if (ft_strcmp(token->value, ">") == 0)
		return (REDIR_OUT);
	else if (ft_strcmp(token->value, ">>") == 0)
		return (REDIR_APPEND);
	else if (ft_strcmp(token->value, "<<") == 0)
		return (REDIR_HEREDOC);
	return ((t_redir_type)-1); // Inválido
}

static t_token	*redir_syntax_error(t_token *current_token, t_struct *mini,
		t_command *cmd_head)
{
	const char	*token_value;

	if (current_token)
		token_value = current_token->value;
	else
		token_value = "newline";
	fprintf(stderr, "minishell: syntax error near unexpected token `%s`\n",
		token_value);
	parser_error_cleanup(cmd_head, mini);
	return (NULL);
}

// Procesa un token de redirección.
static t_token	*process_redirection(t_token *current_token,
		t_command *current_cmd, t_struct *mini, t_command *cmd_head)
{
	t_redir_type	type;
	t_redirection	*new_redir;

	if (!current_token || !current_token->next
		|| current_token->next->type != WORD)
		return (redir_syntax_error(current_token, mini, cmd_head));
	type = get_redirection_type(current_token);
	if (type == (t_redir_type)-1)
	{
		parser_error_cleanup(cmd_head, mini);
		return (NULL);
	}
	new_redir = create_redirection_node(type, current_token->next->value);
	if (!new_redir)
	{
		parser_error_cleanup(cmd_head, mini);
		return (NULL);
	}
	add_redir_to_command(current_cmd, new_redir);
	return (current_token->next->next);
}

// Procesa un token de tipo WORD (argumento o comando).
static t_token	*process_word(t_token *current_token, t_command *current_cmd,
		t_struct *mini, t_command *cmd_head)
{
	if (!add_arg_to_command(current_cmd, current_token->value))
	{
		parser_error_cleanup(cmd_head, mini);
		return (NULL);
	}
	return (current_token->next);
}

static t_command	*pipe_syntax_error(t_struct *mini, t_command *cmd_head)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
	parser_error_cleanup(cmd_head, mini);
	return (NULL);
}

// Procesa un token PIPE.
static t_command	*process_pipe(t_token *current_token,
		t_command *current_cmd, t_struct *mini, t_command *cmd_head)
{
	t_command	*new_cmd;

	if (!current_token->next)
		return (pipe_syntax_error(mini, cmd_head));
	if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0)
		return (pipe_syntax_error(mini, cmd_head));
	new_cmd = create_command_node();
	if (!new_cmd)
	{
		parser_error_cleanup(cmd_head, mini);
		return (NULL);
	}
	current_cmd->next = new_cmd;
	new_cmd->prev = current_cmd;
	return (new_cmd);
}

// Maneja el error de un pipe al inicio de la lista de tokens.
static t_token	*leading_pipe_error(t_struct *mini, t_command *cmd_head)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
	parser_error_cleanup(cmd_head, mini);
	return (NULL);
}

// Decide cómo procesar el token actual y avanza un puntero de token o comando.
static t_token	*process_token_node(t_token *current_token,
		t_command **current_cmd, t_struct *mini, t_command *cmd_head,
		t_token *token_list)
{
	if (current_token->type == PIPE && current_token == token_list)
		return (leading_pipe_error(mini, cmd_head));
	if (current_token->type == WORD)
		return (process_word(current_token, *current_cmd, mini, cmd_head));
	else if (current_token->type == IN || current_token->type == OUT
		|| current_token->type == APPE_OUT || current_token->type == HEREDOC)
		return (process_redirection(current_token, *current_cmd, mini,
				cmd_head));
	else if (current_token->type == PIPE)
	{
		*current_cmd = process_pipe(current_token, *current_cmd, mini,
				cmd_head);
		return (current_token->next);
	}
	parser_error_cleanup(cmd_head, mini);
	return (NULL);
}

// Maneja el resultado del bucle de parsing.
static int	parse_loop_result(t_token *current_token, t_command *cmd_head,
		t_struct *mini)
{
	if (!current_token && cmd_head && mini->last_exit_status == 0)
		return (1); // Éxito, bucle debe romperse.
	if (!current_token && mini->last_exit_status != 0)
		return (0); // Error, parser debe retornar NULL.
	return (-1);    // Continuar el bucle.
}

// Maneja el error de pipe final.
static t_command	*trailing_pipe_error(t_command *cmd_head, t_struct *mini)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
	parser_error_cleanup(cmd_head, mini);
	return (NULL);
}

// Auxiliares
static t_command	*init_parse(t_token *token_list, t_struct *mini)
{
	t_command	*cmd_head;

	if (!token_list)
		return (NULL);
	cmd_head = create_command_node();
	if (!cmd_head)
	{
		parser_error_cleanup(cmd_head, mini);
		return (NULL);
	}
	return (cmd_head);
}

static t_command	*validate_parse_end(t_command *cmd_head,
	t_command *current_cmd, t_struct *mini)
{
	if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0
		&& cmd_head != current_cmd)
		return (trailing_pipe_error(cmd_head, mini));
	return (cmd_head);
}

t_command	*parse_input(t_token *token_list, t_struct *mini)
{
	t_command	*cmd_head;
	t_command	*current_cmd;
	t_token		*current_token;
	int			loop_res;

	cmd_head = init_parse(token_list, mini);
	if (!cmd_head)
		return (NULL);
	current_cmd = cmd_head;
	current_token = token_list;
	while (current_token)
	{
		current_token = process_token_node(current_token, &current_cmd,
				mini, cmd_head, token_list);
		if (!current_token && mini->last_exit_status == 258)
			return (NULL);
		loop_res = parse_loop_result(current_token, cmd_head, mini);
		if (loop_res == 1)
			break ;
		else if (loop_res == 0)
			return (NULL);
	}
	return (validate_parse_end(cmd_head, current_cmd, mini));
}


// Libera argumentos de un comando.
static void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
}

// Libera la lista de redirecciones de un comando.
static void	free_redirections(t_redirection *redir)
{
	t_redirection	*next_redir;

	while (redir)
	{
		next_redir = redir->next;
		if (redir->file)
			free(redir->file);
		free(redir);
		redir = next_redir;
	}
}

// Recorre la lista de comandos y llama a las funciones auxiliares.
void	free_commands(t_command *head)
{
	t_command *cmd_tmp;

	while (head)
	{
		cmd_tmp = head;
		head = head->next;
		free_args(cmd_tmp->args);
		free_redirections(cmd_tmp->redirections);
		if (cmd_tmp->full_path)
			free(cmd_tmp->full_path);
		if (cmd_tmp->heredoc_fd != -1)
			close(cmd_tmp->heredoc_fd);
		free(cmd_tmp);
	}
}