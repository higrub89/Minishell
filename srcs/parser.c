
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
// Crea un nuevo nodo de redirección.
t_redirection	*create_redirection_node(t_redir_type type, char *file)
{
    t_redirection	*new_redir;

    new_redir = (t_redirection *)ft_calloc(1, sizeof(t_redirection));
    if (!new_redir)
    {
        perror("minishell: malloc failed in create_redirection_node");
        return (NULL);
    }
    new_redir->type = type;
    new_redir->file = ft_strdup(file);
    if (!new_redir->file)
    {
        perror("minishell: ft_strdup failed in create_redirection_node");
        free(new_redir);
        return (NULL);
    }
    new_redir->expand_heredoc_content = (type == REDIR_HEREDOC && !ft_strchr(file, '\'') && !ft_strchr(file, '\"'));
    new_redir->next = NULL;
    return (new_redir);
}

// Añade una redirección a la lista de redirecciones de un comando.
void	add_redir_to_command(t_command *cmd, t_redirection *new_redir)
{
    t_redirection	*last_redir;

    if (!cmd || !new_redir)
        return;
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
    {
        perror("minishell: ft_strdup failed for new arg");
        free(new_args);
        return (0);
    }
    new_args[cmd->num_args + 1] = NULL;
    if (cmd->args)
        free(cmd->args);
    cmd->args = new_args;
    cmd->num_args++;
    return (1);
}

// Función para limpiar en caso de error de parsing.
static t_command	*handle_parse_error(t_command *cmd_list_head, t_struct *mini)
{
    if (cmd_list_head)
        free_commands(cmd_list_head);
    mini->last_exit_status = 258;
    return (NULL);
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

// Procesa un token de redirección.
static t_token	*process_redirection(t_token *current_token,
        t_command *current_cmd, t_struct *mini, t_command *cmd_head)
{
    t_redir_type	type;
    t_redirection	*new_redir;

    if (!current_token || !current_token->next
        || current_token->next->type != WORD)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `%s`\n",
            current_token ? current_token->value : "newline");
        return (handle_parse_error(cmd_head, mini));
    }
    type = get_redirection_type(current_token);
    if (type == (t_redir_type)-1)
        return (handle_parse_error(cmd_head, mini));
    new_redir = create_redirection_node(type, current_token->next->value);
    if (!new_redir)
        return (handle_parse_error(cmd_head, mini));
    add_redir_to_command(current_cmd, new_redir);
    return (current_token->next->next);
}

// Procesa un token de tipo WORD (argumento o comando).
static t_token	*process_word(t_token *current_token, t_command *current_cmd,
        t_struct *mini, t_command *cmd_head)
{
    if (!add_arg_to_command(current_cmd, current_token->value))
        return (handle_parse_error(cmd_head, mini)); // Error malloc.
    return (current_token->next);
}

// Procesa un token PIPE.
static t_command	*process_pipe(t_token *current_token,
        t_command *current_cmd, t_struct *mini, t_command *cmd_head)
{
    t_command	*new_cmd;

    if (!current_token->next) // Pipe sin comando después
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
        return (handle_parse_error(cmd_head, mini));
    }
    if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0) // Empty command before pipe
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
        return (handle_parse_error(cmd_head, mini));
    }
    new_cmd = create_command_node();
    if (!new_cmd)
        return (handle_parse_error(cmd_head, mini));
    current_cmd->next = new_cmd;
    new_cmd->prev = current_cmd;
    return (new_cmd);
}

// Decide cómo procesar el token actual y avanza un puntero de token o comando.
static t_token	*process_token_node(t_token *current_token, t_command **current_cmd,
        t_struct *mini, t_command *cmd_head, t_token *token_list)
{
    if (current_token->type == PIPE && current_token == token_list) // Leading pipe
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
        return (handle_parse_error(cmd_head, mini));
    }
    if (current_token->type == WORD)
        return (process_word(current_token, *current_cmd, mini, cmd_head));
    else if (current_token->type == IN || current_token->type == OUT
        || current_token->type == APPE_OUT || current_token->type == HEREDOC)
        return (process_redirection(current_token, *current_cmd, mini, cmd_head));
    else if (current_token->type == PIPE)
    {
        *current_cmd = process_pipe(current_token, *current_cmd, mini, cmd_head);
        return (current_token->next);
    }
    return (handle_parse_error(cmd_head, mini));
}

// Función principal de parsing.
t_command	*parse_input(t_token *token_list, t_struct *mini)
{
    t_command	*cmd_head;
    t_command	*current_cmd;
    t_token		*current_token;

    if (!token_list)
        return (NULL);
    cmd_head = create_command_node();
    if (!cmd_head)
        return (handle_parse_error(NULL, mini));
    current_cmd = cmd_head;
    current_token = token_list;
    while (current_token)
    {
        current_token = process_token_node(current_token, &current_cmd, mini, cmd_head, token_list);
        if (!current_token && cmd_head && mini->last_exit_status == 0)
            break;
        if (!current_token && mini->last_exit_status != 0)
            return (NULL);
    }
    // Check for trailing pipe (last cmd empty)
    if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0 && cmd_head != current_cmd)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `|`\n");
        return (handle_parse_error(cmd_head, mini));
    }
    return (cmd_head);
}

// Libera argumentos de un comando.
static void	free_args(char **args)
{
    int	i;

    if (!args)
        return;
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
    t_command	*cmd_tmp;

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