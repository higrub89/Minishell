#include "../inc/parser.h"

// --- FUNCIONES DE CREACIÓN DE NODOS ---

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

static char *remove_quotes(char *str)
{
    char *new_str;
    int len;

    if (!str)
        return (NULL);
    len = ft_strlen(str);
    if (len > 1 && ((str[0] == '\'' && str[len - 1] == '\'') ||
        (str[0] == '\"' && str[len - 1] == '\"')))
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
	char *file_content;

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

	if (!cmd || !arg) return (0);
	new_args = (char **)ft_calloc(cmd->num_args + 2, sizeof(char *));
	if (!new_args)
		return (perror("minishell: malloc failed"), 0);
	i = -1;
	while(++i < cmd->num_args)
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

// --- GESTIÓN DE ERRORES Y LIBERACIÓN ---

static void *set_syntax_error(const char *token_value, t_struct *mini)
{
    fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n", token_value);
    mini->last_exit_status = 258;
    return (NULL);
}

static void	free_args(char **args)
{
	int	i;
	if (!args) return;
	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
}

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

// --- FUNCIONES DE PROCESAMIENTO ---

static t_redir_type	get_redirection_type(t_token *token)
{
	if (token->type == IN) return (REDIR_IN);
	if (token->type == OUT) return (REDIR_OUT);
	if (token->type == APPE_OUT) return (REDIR_APPEND);
	if (token->type == HEREDOC) return (REDIR_HEREDOC);
	return ((t_redir_type)-1);
}

static t_token	*process_redirection(t_token *tok, t_command *cmd, t_struct *mini)
{
	t_redirection	*new_redir;

	if (!tok->next || tok->next->type != WORD)
		return (set_syntax_error(tok->next ? tok->next->value : "newline", mini));
	new_redir = create_redirection_node(get_redirection_type(tok), tok->next->value);
	if (!new_redir)
		return (NULL);
	add_redir_to_command(cmd, new_redir);
	return (tok->next->next);
}

// --- FUNCIÓN PRINCIPAL DEL PARSER (REFACTORIZADA) ---

t_command	*parse_input(t_token *token_list, t_struct *mini)
{
	t_command	*cmd_head;
	t_command	*current_cmd;
	t_token		*tok;

	if (!token_list) return (NULL);
	mini->last_exit_status = 0;
	cmd_head = create_command_node();
	if (!cmd_head) return (NULL);

	if (token_list->type == PIPE)
        return (free_commands(cmd_head), set_syntax_error("|", mini));

	current_cmd = cmd_head;
	tok = token_list;
	while (tok)
	{
		if (tok->type == WORD)
        {
			if (!add_arg_to_command(current_cmd, tok->value))
                return (free_commands(cmd_head), NULL);
            tok = tok->next;
        }
		else if (tok->type >= IN && tok->type <= HEREDOC)
			tok = process_redirection(tok, current_cmd, mini);
		else if (tok->type == PIPE)
        {
			if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0)
                return (free_commands(cmd_head), set_syntax_error("|", mini));
			tok = tok->next;
			if (!tok || tok->type == PIPE)
                return (free_commands(cmd_head), set_syntax_error(tok ? tok->value : "newline", mini));
			current_cmd->next = create_command_node();
			if (!current_cmd->next)
                return (free_commands(cmd_head), NULL);
			current_cmd->next->prev = current_cmd;
			current_cmd = current_cmd->next;
        }
		if (mini->last_exit_status == 258)
            return (free_commands(cmd_head), NULL);
	}
	if (current_cmd->num_args == 0 && current_cmd->num_redirections == 0)
		return (free_commands(cmd_head), set_syntax_error("newline", mini));
	return (cmd_head);
}