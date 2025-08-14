
#include "../inc/redirection.h"

static volatile sig_atomic_t	g_last_signal = 0; // Global para SIGINT

// Handler para SIGINT en heredoc
static void	handle_sigint(int sig)
{
	(void)sig;
	g_last_signal = 1;
}

// Manejo de señal durante heredoc
static int	handle_heredoc_signal(t_struct *mini, char *line)
{
	free(line);
	mini->last_exit_status = 130;
	return (1);
}

// Manejo de EOF (Ctrl+D) durante heredoc
static int	handle_heredoc_eof(t_struct *mini, char *line)
{
	free(line);
	//mini->last_exit_status = 1;
	return (1);
}

// Expande variables si es necesario o hace strdup
static char	*get_expanded_line(char *line, t_redirection *heredoc,
		t_struct *mini)
{
	char	*expanded_line;

	if (heredoc->expand_heredoc_content)
		expanded_line = expand_string(line, mini);
	else
		expanded_line = ft_strdup(line);
	free(line);
	return (expanded_line);
}

// Escribe línea en pipe, maneja fallo de malloc
static int	write_heredoc_line(char *expanded_line, int pipe_fd[2],
		t_struct *mini)
{
	if (!expanded_line)
	{
		perror("minishell: malloc failed during heredoc expansion");
		mini->last_exit_status = 1;
		return (1);
	}
	write(pipe_fd[1], expanded_line, ft_strlen(expanded_line));
	write(pipe_fd[1], "\n", 1);
	free(expanded_line);
	return (0);
}

// Bucle principal del heredoc
static int	heredoc_loop(t_redirection *heredoc, int pipe_fd[2], t_struct *mini)
{
	char	*line;
	int		ret_val;

	ret_val = 0;
	while (1)
	{
		line = readline(">");
		if (g_last_signal)
			ret_val = handle_heredoc_signal(mini, line);
		else if (!line)
			ret_val = handle_heredoc_eof(mini, line);
		else if (ft_strcmp(line, heredoc->file) == 0)
		{
			free(line);
			break ;
		}
		else
			ret_val = write_heredoc_line(get_expanded_line(line, heredoc, mini),
					pipe_fd, mini);
		if (ret_val)
			break ;
	}
	return (ret_val);
}

// Función principal para procesar un heredoc
static int	process_single_heredoc(t_redirection *heredoc, int pipe_fd[2],
		t_struct *mini)
{
	int	ret_val;

	signal(SIGINT, handle_sigint);
	g_last_signal = 0;
	ret_val = heredoc_loop(heredoc, pipe_fd, mini);
	if (ret_val)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}
	signal(SIGINT, SIG_DFL);
	return (ret_val);
}

// Devuelve el último heredoc de la lista de redirecciones de un comando
static t_redirection	*get_last_heredoc(t_redirection *redir)
{
	t_redirection	*last;

	last = NULL;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
			last = redir;
		redir = redir->next;
	}
	return (last);
}

// Procesa un heredoc de un comando, devuelve 1 si hay error
static int	process_command_heredoc(t_command *cmd, t_struct *mini)
{
	t_redirection	*heredoc;
	int				pipe_fd[2];

	heredoc = get_last_heredoc(cmd->redirections);
	if (!heredoc)
		return (0);
	if (pipe(pipe_fd) == -1)
	{
		perror("minishell: pipe for heredoc");
		mini->last_exit_status = 1;
		return (1);
	}
	if (process_single_heredoc(heredoc, pipe_fd, mini))
		return (1);
	close(pipe_fd[1]);
	cmd->heredoc_fd = pipe_fd[0];
	return (0);
}

// Procesa todos los heredocs de la lista de comandos
int	process_heredoc_input(t_command *cmd_list, t_struct *mini)
{
	t_command *cmd;

	cmd = cmd_list;
	while (cmd)
	{
		if (process_command_heredoc(cmd, mini))
			return (1);
		cmd = cmd->next;
	}
	return (0);
}