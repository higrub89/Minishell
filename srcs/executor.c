#include "../inc/builtins.h"
#include "../inc/executor.h"

static void add_slash_to_paths(char **paths)
{
    int i;
    char *tmp;
    size_t len;

    i = 0;
    while (paths && paths[i])
    {
        len = ft_strlen(paths[i]);
        if (paths[i][len - 1] != '/')
        {
            tmp = ft_strjoin(paths[i], "/");
            if (!tmp)
            {
                perror("minishell: malloc error in add_slash_to_path");
                return ;
            }
            free(paths[i]);
            paths[i] = tmp;
        }
        i++;
    }
}

static char *get_path_var(char **envp)
{
    int i;

    i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i] + 5);
        i++;
    }
    return (NULL);
}

static char **get_default_paths(void)
{
    char **paths;

    paths = ft_split("/usr/local/bin:/usr/bin:/bin", ':');
    if (!paths)
        return (NULL);
    add_slash_to_paths(paths);
    return (paths);
}

char **get_paths(char **envp)
{
    char *path_var;
    char **paths;

    path_var = get_path_var(envp);
    if (!path_var || *path_var == '\0')
        return (get_default_paths());
    paths = ft_split(path_var, ':');
    if (!paths)
        return (NULL);
    add_slash_to_paths(paths);
    return (paths);
}

static char *check_direct_path(char *cmd)
{
    if (access(cmd, X_OK) == 0)
        return (ft_strdup(cmd));
    return (NULL);
}

char *find_cmd_path(char **cmd_args, char **paths)
{
    int i;
    char *full_path;

    if (!cmd_args || !cmd_args[0])
        return (NULL);
    if (ft_strchr(cmd_args[0], '/'))
        return (check_direct_path(cmd_args[0]));
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        full_path = ft_strjoin(paths[i], cmd_args[0]);
        if (!full_path)
            return (NULL);
        if (access(full_path, X_OK) == 0)
            return (full_path);
        free(full_path);
        i++;
    }
    return (NULL);
}

static int dup2_and_close(int oldfd, int newfd)
{
    if (oldfd == newfd)
        return (0);
    if (dup2(oldfd, newfd) == -1)
    {
        perror("minishell: dup2");
        close(oldfd);
        return (1);
    }
    close(oldfd);
    return (0);
}

static int open_redirection_file(t_redirection *redir)
{
    if (redir->type == REDIR_IN)
        return (open(redir->file, O_RDONLY));
    else if (redir->type == REDIR_OUT)
        return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
    else if (redir->type == REDIR_APPEND)
        return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
    return (-1);
}

static int handle_single_redirection(t_redirection *redir, int *last_fd)
{
    int new_fd;

    if (*last_fd != -1)
        close(*last_fd);
    new_fd = open_redirection_file(redir);
    if (new_fd == -1)
    {
        perror(redir->file);
        return (-1);
    }
    *last_fd = new_fd;
    return (0);
}

static int handle_redir(t_redirection *redir, int *last_fd)
{
    if (handle_single_redirection(redir, last_fd) == -1)
    {
        if (*last_fd != -1)
            close(*last_fd);
        return (1);
    }
    return (0);
}

static int apply_final_fds(int last_in_fd, int last_out_fd)
{
    if (last_in_fd != -1 && dup2_and_close(last_in_fd, STDIN_FILENO))
        return (1);
    if (last_out_fd != -1 && dup2_and_close(last_out_fd, STDOUT_FILENO))
        return (1);
    return (0);
}

static int process_redirections(t_command *cmd, int *last_in_fd,
    int *last_out_fd)
{
    t_redirection *redir;

    redir = cmd->redirections;
    while (redir)
    {
        if (redir->type != REDIR_HEREDOC)
        {
            if ((redir->type == REDIR_IN && handle_redir(redir, last_in_fd))
                || ((redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
                    && handle_redir(redir, last_out_fd)))
                return (1);
        }
        redir = redir->next;
    }
    return (0);
}

int handle_redirections_in_child(t_command *cmd)
{
    int last_in_fd;
    int last_out_fd;

    last_in_fd = (cmd->heredoc_fd != -1) ? cmd->heredoc_fd : -1;
    last_out_fd = -1;
    if (process_redirections(cmd, &last_in_fd, &last_out_fd))
        return (1);
    return (apply_final_fds(last_in_fd, last_out_fd));
}

static void setup_child_pipes(t_command *cmd, int *pipe_fds,
    int prev_pipe_in_fd)
{
    if (prev_pipe_in_fd != -1)
    {
        if (dup2_and_close(prev_pipe_in_fd, STDIN_FILENO) != 0)
            exit(1);
    }
    if (cmd->next)
    {
        if (dup2_and_close(pipe_fds[1], STDOUT_FILENO) != 0)
            exit(1);
    }
    if (cmd->next)
    {
        close(pipe_fds[0]);
    }
}

static void execute_external_command(t_command *cmd, t_struct *mini)
{
    char **paths;
    char *cmd_path;

    paths = get_paths(mini->envp);
    cmd_path = find_cmd_path(cmd->args, paths);
    if (!cmd_path)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(cmd->args[0], STDERR_FILENO);
        ft_putendl_fd(": command not found", STDERR_FILENO);
        free_str_array(paths);
        exit(127);
    }
    execve(cmd_path, cmd->args, mini->envp);
    perror("minishell: execve failed");
    free(cmd_path);
    free_str_array(paths);
    exit(126);
}

static void child_execute_command(t_command *cmd, t_struct *mini)
{
    if (handle_redirections_in_child(cmd) != 0)
        exit(1);
    if (!cmd->args || !cmd->args[0] || cmd->args[0][0] == '\0')
        exit(0);
    if (is_builtin(cmd->args[0]))
    {
        execute_builtin(cmd, mini);
        exit(mini->last_exit_status);
    }
    execute_external_command(cmd, mini);
}

static void parent_cleanup(t_command *cmd, int *pipe_fds, int *prev_pipe_in_fd,
    pid_t child_pid, pid_t *child_pids, int cmd_idx)
{
    child_pids[cmd_idx] = child_pid;
    if (*prev_pipe_in_fd != -1)
        close(*prev_pipe_in_fd);
    if (cmd->next)
    {
        close(pipe_fds[1]);
        *prev_pipe_in_fd = pipe_fds[0];
    }
    else
        *prev_pipe_in_fd = -1;
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1;
    }
}

// CORREGIDO: Esta función ya NO libera child_pids
static void wait_for_children(pid_t *child_pids, int num_commands,
    t_struct *mini)
{
    int status;
    int i;

    i = 0;
    while (i < num_commands)
    {
        waitpid(child_pids[i], &status, 0);
        if (i == num_commands - 1)
        {
            if (WIFEXITED(status))
                mini->last_exit_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                mini->last_exit_status = 128 + WTERMSIG(status);
        }
        i++;
    }
}

static int init_pipeline(t_command *commands, pid_t **child_pids_ptr)
{
    t_command *current_cmd;
    int num_commands;

    num_commands = 0;
    current_cmd = commands;
    while (current_cmd)
    {
        num_commands++;
        current_cmd = current_cmd->next;
    }
    *child_pids_ptr = ft_calloc(num_commands + 1, sizeof(pid_t));
    if (!*child_pids_ptr)
    {
        perror("minishell: calloc child_pids");
        return (-1);
    }
    return (num_commands);
}

static void handle_fork_error(int prev_pipe_in_fd, int *pipe_fds,
    t_command *current_cmd, pid_t *child_pids, t_struct *mini)
{
    perror("minishell: fork");
    mini->last_exit_status = 1;
    if (prev_pipe_in_fd != -1)
        close(prev_pipe_in_fd);
    if (current_cmd && current_cmd->next)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }
    if (current_cmd && current_cmd->heredoc_fd != -1)
    {
        close(current_cmd->heredoc_fd);
        current_cmd->heredoc_fd = -1;
    }
    free(child_pids);
}

static void close_heredoc(t_command *cmd)
{
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1;
    }
}

static void restore_fds(int stdin_fd, int stdout_fd)
{
    dup2(stdin_fd, STDIN_FILENO);
    close(stdin_fd);
    dup2(stdout_fd, STDOUT_FILENO);
    close(stdout_fd);
}

// Nueva función auxiliar para manejar redirecciones sin un comando
static int handle_single_redirection_only(t_command *cmd, t_struct *mini)
{
    int orig_stdin;
    int orig_stdout;
    int result;

    orig_stdin = dup(STDIN_FILENO);
    orig_stdout = dup(STDOUT_FILENO);
    if (orig_stdin == -1 || orig_stdout == -1)
        return (perror("minishell: dup original fds"), 1);

    result = handle_redirections_in_child(cmd);

    restore_fds(orig_stdin, orig_stdout);
    
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1;
    }

    mini->last_exit_status = result;
    return (result);
}

static int handle_single_builtin(t_command *cmd, t_struct *mini)
{
    int orig_stdin;
    int orig_stdout;

    set_signals(NON_INTERACTIVE);
    orig_stdin = dup(STDIN_FILENO);
    orig_stdout = dup(STDOUT_FILENO);
    if (orig_stdin == -1 || orig_stdout == -1)
        return (perror("minishell: dup original fds"), 1);
    if (handle_redirections_in_child(cmd) != 0)
    {
        restore_fds(orig_stdin, orig_stdout);
        close_heredoc(cmd);
        return (1);
    }
    execute_builtin(cmd, mini);
    restore_fds(orig_stdin, orig_stdout);
    close_heredoc(cmd);
    return (0);
}

static pid_t create_pipe_and_fork(t_command *cmd, int pipe_fd[2])
{
    if (cmd->next && pipe(pipe_fd) == -1)
        return (perror("pipe"), -1);
    return (fork());
}

static void child_process(t_command *cmd, int pipe_fd[2], int prev_fd,
    t_struct *mini)
{
    set_signals(CHILD);
    setup_child_pipes(cmd, pipe_fd, prev_fd);
    child_execute_command(cmd, mini);
}

static void parent_process(t_command *cmd, int pipe_fd[2], int *prev_fd,
    pid_t pid, pid_t *child_pids, int cmd_idx)
{
    parent_cleanup(cmd, pipe_fd, prev_fd, pid, child_pids, cmd_idx);
}

static int execute_pipeline(t_command *cmds, t_struct *mini, pid_t *child_pids,
    int num_commands)
{
    t_command *curr;
    int prev_fd;
    int cmd_idx;
    int pipe_fd[2];
    pid_t pid;

    set_signals(NON_INTERACTIVE);
    curr = cmds;
    prev_fd = -1;
    cmd_idx = 0;
    while (curr)
    {
        pid = create_pipe_and_fork(curr, pipe_fd);
        if (pid == -1)
        {
            handle_fork_error(prev_fd, pipe_fd, curr, child_pids, mini);
            return (1);
        }
        if (pid == 0)
            child_process(curr, pipe_fd, prev_fd, mini);
        else
            parent_process(curr, pipe_fd, &prev_fd, pid, child_pids, cmd_idx++);
        curr = curr->next;
    }
    wait_for_children(child_pids, num_commands, mini);
    return (0);
}

int execute_commands(t_command *commands, t_struct *mini)
{
    pid_t *child_pids;
    int num_commands;
    int result;

    if (!commands->args || !commands->args[0])
    {
        if (commands->redirections) // AHORA MANEJAMOS ESTE CASO ESPECÍFICO
            return (handle_single_redirection_only(commands, mini));
        else // Ni argumentos ni redirecciones, es una línea vacía
            return (0);
    }
    num_commands = init_pipeline(commands, &child_pids);
    if (num_commands == -1)
        return (mini->last_exit_status = 1, 1);
    if (num_commands == 1 && commands->args && commands->args[0]
        && is_builtin(commands->args[0]))
        result = handle_single_builtin(commands, mini);
    else
        result = execute_pipeline(commands, mini, child_pids, num_commands);
    free(child_pids);
    return (result);
}

int is_builtin(char *cmd_name)
{
    if (!cmd_name)
        return (0);
    if (ft_strcmp(cmd_name, "echo") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "cd") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "pwd") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "export") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "unset") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "env") == 0)
        return (1);
    if (ft_strcmp(cmd_name, "exit") == 0)
        return (1);
    return (0);
}

int execute_builtin(t_command *cmd, t_struct *mini)
{
    if (!cmd || !cmd->args || !cmd->args[0])
    {
        mini->last_exit_status = 1;
        return (1);
    }
    if (ft_strcmp(cmd->args[0], "echo") == 0)
        return (ft_echo(cmd->args));
    else if (ft_strcmp(cmd->args[0], "cd") == 0)
        return (ft_cd(mini, cmd->args));
    else if (ft_strcmp(cmd->args[0], "pwd") == 0)
        return (ft_pwd(mini));
    else if (ft_strcmp(cmd->args[0], "export") == 0)
        return (ft_export(mini, cmd->args));
    else if (ft_strcmp(cmd->args[0], "unset") == 0)
        return (ft_unset(mini, cmd->args));
    else if (ft_strcmp(cmd->args[0], "env") == 0)
        return (ft_env(mini, cmd->args));
    else if (ft_strcmp(cmd->args[0], "exit") == 0)
        return (ft_exit(mini, cmd->args));
    mini->last_exit_status = 1;
    return (1);
}