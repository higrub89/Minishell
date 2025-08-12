#include "../inc/executor.h"

static void	add_slash_to_paths(char **paths)
{
	int		i;
	char	*tmp;
	size_t	len;

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

char    **get_paths(char **envp)
{
    int     i;
    char    *path_var;
    char    **paths;

    i = 0;
    path_var = NULL;
    while (envp && envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            path_var = envp[i] + 5;
            break ;
        }
        i++;
    }
    if (!path_var || *path_var == '\0') 
    {
        paths = ft_split("/usr/local/bin:/usr/bin:/bin", ':'); // O solo "/bin:/usr/bin"
        if (!paths) // Manejo de error para ft_split
            return (NULL);
        add_slash_to_paths(paths);
        return (paths);
    }
    paths = ft_split(path_var, ':');
    if (!paths) // Manejo de error para ft_split
        return (NULL);
    add_slash_to_paths(paths);
    return (paths);
}

static char	*check_direct_path(char *cmd)
{
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	return (NULL);
}

char	*find_cmd_path(char **cmd_args, char **paths)
{
	int		i;
	char	*full_path;

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

void	free_str_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

// Duplicates file descriptor to standard stream and closes original.
static int dup2_and_close(int oldfd, int newfd)
{
    if (oldfd == newfd) // Already the same, no need to dup2
        return (0);
    if (dup2(oldfd, newfd) == -1)
    {
        perror("minishell: dup2");
        close(oldfd); // Ensure oldfd is closed on error
        return (1);
    }
    close(oldfd);
    return (0);
}

// Opens a file based on redirection type, returns fd.
static int open_redirection_file(t_redirection *redir)
{
    if (redir->type == REDIR_IN)
        return (open(redir->file, O_RDONLY));
    else if (redir->type == REDIR_OUT)
        return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
    else if (redir->type == REDIR_APPEND)
        return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
    return (-1); // Should not happen for valid types
}

int handle_redirections_in_child(t_command *cmd)
{
    t_redirection   *redir;
    int             fd;
    int             last_in_fd; // Stores the final input FD (from file or heredoc)
    int             last_out_fd; // Stores the final output FD

    last_in_fd = -1;
    last_out_fd = -1;

    // 1. Prioritize heredoc if present (parser should have set cmd->heredoc_fd)
    if (cmd->heredoc_fd != -1)
        last_in_fd = cmd->heredoc_fd;

    // 2. Process all file redirections
    redir = cmd->redirections;
    while (redir)
    {
        // Skip heredoc types in this loop, they are handled via cmd->heredoc_fd
        if (redir->type == REDIR_HEREDOC)
        {
            redir = redir->next;
            continue;
        }

        fd = open_redirection_file(redir);
        if (fd == -1)
        {
            perror(redir->file);
            // Close any FDs already opened by previous redirections for this command
            if (last_in_fd != -1 && last_in_fd != cmd->heredoc_fd) close(last_in_fd);
            if (last_out_fd != -1) close(last_out_fd);
            return (1); // Indicate error
        }

        if (redir->type == REDIR_IN)
        {
            if (last_in_fd != -1) // Close previous input source if a new one is found
                close(last_in_fd);
            last_in_fd = fd;
        }
        else // REDIR_OUT or REDIR_APPEND
        {
            if (last_out_fd != -1) // Close previous output destination if a new one is found
                close(last_out_fd);
            last_out_fd = fd;
        }
        redir = redir->next;
    }

    // 3. Apply the final input and output FDs
    if (last_in_fd != -1)
    {
        if (dup2_and_close(last_in_fd, STDIN_FILENO) != 0) return (1);
    }
    if (last_out_fd != -1)
    {
        if (dup2_and_close(last_out_fd, STDOUT_FILENO) != 0) return (1);
    }
    return (0);
}

// Sets up pipe redirections for the child process.
static void setup_child_pipes(t_command *cmd, int *pipe_fds,
        int prev_pipe_in_fd)
{
    // Configure STDIN from previous pipe if applicable
    if (prev_pipe_in_fd != -1)
    {
        if (dup2_and_close(prev_pipe_in_fd, STDIN_FILENO) != 0)
            exit(1); // Exit if dup2 fails
    }
    // Configure STDOUT to current pipe if applicable
    if (cmd->next)
    {
        if (dup2_and_close(pipe_fds[1], STDOUT_FILENO) != 0)
            exit(1); // Exit if dup2 fails
    }
    // Close unused pipe ends in child
    if (cmd->next)
    {
        close(pipe_fds[0]); // Read end of current pipe is not used by this child
    }
}

// Executes a command in the child process.
static void child_execute_command(t_command *cmd, t_struct *mini)
{
    char        **paths;
    char        *cmd_path;

    // Handle all redirections (heredoc, input files, output files)
    if (handle_redirections_in_child(cmd) != 0)
        exit(1); // Exit child on redirection error

    if (!cmd->args || !cmd->args[0]) // Empty command (e.g., just redirections)
        exit(0);

    // TODO: Builtin check. If it's a builtin, execute it and exit.
    // These functions must be defined (even as placeholders) for compilation.
    if (is_builtin(cmd->args[0]))
    {
        execute_builtin(cmd, mini); // This should update mini->last_exit_status
        exit(mini->last_exit_status);
    }
    
    // External command execution
    paths = get_paths(mini->envp);
    cmd_path = find_cmd_path(cmd->args, paths); // Pasamos cmd->args completo

    if (!cmd_path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", cmd->args[0]);
        free_str_array(paths);
        exit(127); // Command not found
    }
    execve(cmd_path, cmd->args, mini->envp);
    // If execve returns, it failed
    perror("minishell: execve");
    free(cmd_path); // These frees are only reached if execve fails
    free_str_array(paths);
    exit(126); // Command not executable (e.g., permission denied)
}


// --- Parent Process Specific Logic ---

// Performs pipe and heredoc cleanup in the parent process after a fork.
static void parent_cleanup(t_command *cmd, int *pipe_fds, int *prev_pipe_in_fd,
        pid_t child_pid, pid_t *child_pids, int cmd_idx)
{
    child_pids[cmd_idx] = child_pid; // Store child PID
    if (*prev_pipe_in_fd != -1)
        close(*prev_pipe_in_fd); // Close read end of previous pipe

    if (cmd->next)
    {
        close(pipe_fds[1]); // Close write end of current pipe
        *prev_pipe_in_fd = pipe_fds[0]; // Read end becomes input for next
    }
    else // Last command in pipe
    {
        if (*prev_pipe_in_fd != -1)
            close(*prev_pipe_in_fd); // Close final pipe input
        *prev_pipe_in_fd = -1;
    }
    // Close heredoc_fd in parent (it was opened by parser, child duplicated or ignored it)
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1; // Mark as closed
    }
}

// Waits for all child processes and updates last_exit_status.
static void wait_for_children(pid_t *child_pids, int num_commands,
        t_struct *mini)
{
    int status;
    int i;

    i = 0;
    while (i < num_commands)
    {
        waitpid(child_pids[i], &status, 0);
        if (i == num_commands - 1) // Only last command's status matters for minishell exit status
        {
            if (WIFEXITED(status))
                mini->last_exit_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                mini->last_exit_status = 128 + WTERMSIG(status);
        }
        i++;
    }
    free(child_pids);
}

// Initializes pipeline variables and allocates PIDs array.
static int  init_pipeline(t_command *commands, pid_t **child_pids_ptr)
{
    t_command   *current_cmd;
    int         num_commands;

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
        return (-1); // Indicate error
    }
    return (num_commands);
}

// Handles fork errors, cleaning up resources.
static void handle_fork_error(int prev_pipe_in_fd, int *pipe_fds,
        t_command *current_cmd, pid_t *child_pids, t_struct *mini)
{
    perror("minishell: fork");
    mini->last_exit_status = 1;
    if (prev_pipe_in_fd != -1)
        close(prev_pipe_in_fd);
    // Close current pipe ends if they were created before the fork error
    if (current_cmd && current_cmd->next)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }
    // Close current command's heredoc_fd if it exists
    if (current_cmd && current_cmd->heredoc_fd != -1)
    {
        close(current_cmd->heredoc_fd);
        current_cmd->heredoc_fd = -1;
    }
    free(child_pids);
}

// Main function to execute a list of commands (pipeline).
int execute_commands(t_command *commands, t_struct *mini)
{
    t_command   *current_cmd;
    pid_t       pid;
    int         pipe_fds[2];
    int         prev_pipe_in_fd;
    pid_t       *child_pids;
    int         num_commands;
    int         cmd_idx;

    prev_pipe_in_fd = -1;
    num_commands = init_pipeline(commands, &child_pids);
    if (num_commands == -1)
    {
        mini->last_exit_status = 1; // Set status for malloc error
        return (1);
    }
    
    cmd_idx = 0;
    current_cmd = commands;
    while (current_cmd)
    {
        if (current_cmd->next && pipe(pipe_fds) == -1) // Create pipe if not last command
        {
            perror("minishell: pipe");
            mini->last_exit_status = 1;
            free(child_pids);
            // Close any existing prev_pipe_in_fd or heredoc_fd on pipe error
            if (prev_pipe_in_fd != -1) close(prev_pipe_in_fd);
            if (current_cmd->heredoc_fd != -1) close(current_cmd->heredoc_fd);
            return (1);
        }

        pid = fork();
        if (pid == -1) // Fork error
        {
            handle_fork_error(prev_pipe_in_fd, pipe_fds, current_cmd, child_pids, mini);
            return (1);
        }
        else if (pid == 0) // Child process
        {
            setup_child_pipes(current_cmd, pipe_fds, prev_pipe_in_fd);
            child_execute_command(current_cmd, mini); // Executes command (or builtin) and exits
        }
        else // Parent process
        {
            parent_cleanup(current_cmd, pipe_fds, &prev_pipe_in_fd, pid, child_pids, cmd_idx);
            cmd_idx++;
        }
        current_cmd = current_cmd->next;
    }

    wait_for_children(child_pids, num_commands, mini);
    return (0);
}

// --- TEMPORARY PLACEHOLDERS FOR BUILTINS ---
// Estas funciones DEBEN existir para que el código compile,
// pero su implementación real vendrá después.

int is_builtin(char *cmd_name)
{
    (void)cmd_name; // Evita advertencia de parámetro no usado
    // Por ahora, siempre retorna 0 para que todos los comandos vayan a execve.
    return (0); 
}

int execute_builtin(t_command *cmd, t_struct *mini)
{
    (void)cmd;  // Evita advertencia
    (void)mini; // Evita advertencia
    // Placeholder: El builtin no hace nada y el proceso hijo simplemente termina.
    mini->last_exit_status = 0;
    return (0);
}