
#include "../inc/redirection.h"

static volatile sig_atomic_t g_last_signal = 0; // Global para SIGINT

// Handler para SIGINT en heredoc
static void handle_sigint(int sig)
{
    (void)sig;
    g_last_signal = 1;
}

static int process_single_heredoc(t_redirection *heredoc, int pipe_fd[2], t_struct *mini)
{
    char *line;
    char *expanded_line;
    int ret_val = 0;

    signal(SIGINT, handle_sigint);
    g_last_signal = 0;
    while (1)
    {
        line = readline("");
        if (g_last_signal || !line || ft_strcmp(line, heredoc->file) == 0)
        {
            free(line);
            if (g_last_signal)
            {
                mini->last_exit_status = 130;
                ret_val = 1;
            }
            else if (!line)
            {
                mini->last_exit_status = 1;
                ret_val = 1;
            }
            break ;
        }
        if (heredoc->expand_heredoc_content)
            expanded_line = expand_string(line, mini);
        else
            expanded_line = ft_strdup(line);
        free(line);
        if (!expanded_line)
        {
            perror("minishell: malloc failed during heredoc expansion");
            mini->last_exit_status = 1;
            ret_val = 1;
            break ;
        }
        write(pipe_fd[1], expanded_line, ft_strlen(expanded_line));
        write(pipe_fd[1], "\n", 1);
        free(expanded_line);
    }
    if (ret_val != 0) 
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
    }
    signal(SIGINT, SIG_DFL); // Restaurar comportamiento por defecto
    return (ret_val);
}

// Procesa todos los heredocs en la lista de comandos (usa last << per cmd).
int process_heredoc_input(t_command *cmd_list, t_struct *mini)
{
    t_command		*cmd;
    t_redirection	*redir;
    t_redirection	*last_heredoc;
    int				pipe_fd[2];

    cmd = cmd_list;
    while (cmd)
    {
        last_heredoc = NULL;
        redir = cmd->redirections;
        while (redir)
        {
            if (redir->type == REDIR_HEREDOC)
                last_heredoc = redir; // Use last heredoc
            redir = redir->next;
        }
        if (last_heredoc)
        {
            if (pipe(pipe_fd) == -1)
            {
                perror("minishell: pipe for heredoc");
                mini->last_exit_status = 1;
                return (1);
            }
            if (process_single_heredoc(last_heredoc, pipe_fd, mini))
                return (1);
            close(pipe_fd[1]);
            cmd->heredoc_fd = pipe_fd[0];
        }
        cmd = cmd->next;
    }
    return (0);
}