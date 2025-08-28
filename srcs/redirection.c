#include "../inc/redirection.h"
#include "../inc/expander.h" // Necesario para expand_heredoc_line

// Variable global para la señal SIGINT (Ctrl+C)
static volatile sig_atomic_t	g_last_signal = 0;

/**
 * @brief Manejador de señal para SIGINT, específico para el heredoc.
 */
static void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_last_signal = 1;
	// No hacemos exit aquí, solo marcamos la bandera.
	// El bucle de readline lo gestionará.
}

/**
 * @brief Expande variables en una línea leída del heredoc.
 * Llama a la función específica que no elimina comillas.
 */
static char	*get_expanded_line(char *line, t_redirection *heredoc,
		t_struct *mini)
{
	char	*expanded_line;

	if (heredoc->expand_heredoc_content)
	{
		// Usamos la función que expande variables pero PRESERVA las comillas
		expanded_line = expand_heredoc_line(line, mini);
	}
	else
	{
		expanded_line = ft_strdup(line);
	}
	free(line); // La línea original de readline ya no es necesaria
	return (expanded_line);
}

/**
 * @brief Escribe la línea procesada en el pipe del heredoc.
 */
static int	write_heredoc_line(char *expanded_line, int pipe_fd,
		t_struct *mini)
{
	if (!expanded_line)
	{
		perror("minishell: malloc failed during heredoc expansion");
		mini->last_exit_status = 1;
		return (1); // Error
	}
	write(pipe_fd, expanded_line, ft_strlen(expanded_line));
	write(pipe_fd, "\n", 1);
	free(expanded_line);
	return (0); // Éxito
}

/**
 * @brief El bucle principal que lee la entrada para un solo heredoc.
 */
static int	heredoc_loop(t_redirection *heredoc, int pipe_fd, t_struct *mini)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_last_signal)
		{
			free(line);
			mini->last_exit_status = 130; // 128 + 2 (SIGINT)
			return (1); // Error por interrupción
		}
		if (!line) // EOF (Ctrl+D)
		{
			// Esto no es un error, simplemente se terminó la entrada.
			// Bash imprime un warning aquí, nosotros simplemente terminamos.
			return (0);
		}
		if (ft_strcmp(line, heredoc->file) == 0)
		{
			free(line);
			break ; // Delimitador encontrado, salimos del bucle
		}
		if (write_heredoc_line(get_expanded_line(line, heredoc, mini),
				pipe_fd, mini))
			return (1); // Error de malloc
	}
	return (0); // Éxito
}

/**
 * @brief Procesa todos los heredocs de la lista de comandos.
 * Bash abre todos los heredocs, pero solo el último se conecta al stdin del comando.
 */
int	process_heredoc_input(t_command *cmd_list, t_struct *mini)
{
	t_command		*cmd;
	t_redirection	*redir;
	int				pipe_fd[2];

	cmd = cmd_list;
	while (cmd)
	{
		redir = cmd->redirections;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				// Configurar señal y ejecutar el bucle de heredoc
				signal(SIGINT, handle_sigint_heredoc);
				g_last_signal = 0;
				if (pipe(pipe_fd) == -1)
					return (perror("minishell: pipe"), 1);
				if (heredoc_loop(redir, pipe_fd[1], mini))
				{
					// Si hubo error (Ctrl+C o malloc), cerramos todo y retornamos
					close(pipe_fd[0]);
					close(pipe_fd[1]);
					return (1);
				}
				// Restaurar la señal por defecto para el prompt principal
				signal(SIGINT, SIG_DFL);
				close(pipe_fd[1]); // Cerramos el extremo de escritura
				// Si ya había un heredoc_fd (en caso de múltiples <<), lo cerramos
				if (cmd->heredoc_fd != -1)
					close(cmd->heredoc_fd);
				// Guardamos el extremo de lectura. Si hay más <<, este se sobreescribirá.
				cmd->heredoc_fd = pipe_fd[0];
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0); // Todo salió bien
}