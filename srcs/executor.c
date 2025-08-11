#include "../inc/executor.h"

static char **get_paths(char **envp)
{
    int i = 0;
    while(envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            return (ft_split(envp[i] + 5, ':'));
        }
        i++;
    }
    return (ft_split("/bin:/usr/bin", ':'));
}

static char *find_cmd_path(const char *cmd, char **paths)
{
    char *full_path;
    char *temp_path;
    int i;

    if (!cmd)
        return (NULL);

    if (ft_strchr(cmd, '/') != NULL)
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    if (!paths)
        return (NULL);

    i = 0;
    while (paths[i])
    {
        temp_path = ft_strjoin(paths[i], "/");
        if (!temp_path)
            return (NULL);
        full_path = ft_strjoin(temp_path, cmd);
        free(temp_path);
        if (!full_path)
            return (NULL);
        if (access(full_path, X_OK) == 0)
            return (full_path);
        free(full_path);
        i++;
    }
    return (NULL);
}

static void free_str_array(char **arr)
{
    if (!arr)
        return;
    int i;

    i = 0;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}


// ----- Redirection halding ------

// Handles a single redirection, returning the opened file descriptor.
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

// Duplicates file descriptor to standard stream and closes original.
static int dup2_and_close(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) == -1)
    {
        perror("minishell: dup2");
        close(oldfd); // Ensure oldfd is closed on error
        return (1);
    }
    close(oldfd);
    return (0);
}

// Handles all redirections for a command in the child process.
int handle_redirections_in_child(t_command *cmd)
{
    t_redirection *redir;
    int fd;

    // Heredoc input
    if (cmd->heredoc_fd != -1)
    {
        if (dup2_and_close(cmd->heredoc_fd, STDIN_FILENO) != 0)
            return (1);
    }

    redir = cmd->redirections;
    while (redir)
    {
        if (redir->type == REDIR_HEREDOC) // Already handled above
        {
            redir = redir->next;
            continue;
        }
        fd = open_redirection_file(redir);
        if (fd == -1)
        {
            perror(redir->file); // Error opening file
            return (1);
        }
        if (redir->type == REDIR_IN)
        {
            if (dup2_and_close(fd, STDIN_FILENO) != 0) return (1);
        }
        else // REDIR_OUT or REDIR_APPEND
        {
            if (dup2_and_close(fd, STDOUT_FILENO) != 0) return (1);
        }
        redir = redir->next;
    }
    return (0);
}

// --- Child Process Specific Logic ---

// Sets up pipe redirections for the child process.
static void setup_child_pipes(t_command *cmd, int *pipe_fds, int prev_pipe_in_fd)
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
    char **paths;
    char *cmd_path;

    if (handle_redirections_in_child(cmd) != 0)
        exit(1); // Exit child on redirection error

    if (!cmd->args || !cmd->args[0]) // Empty command (e.g., just redirections)
        exit(0);

    // Builtin check. If it's a builtin, execute it and exit.
    if (is_builtin(cmd->args[0]))
    {
        execute_builtin(cmd, mini); // This should update mini->last_exit_status
        exit(mini->last_exit_status);
    }
    
    // External command execution
    paths = get_paths(mini->envp);
    cmd_path = find_cmd_path(cmd->args[0], paths);

    if (!cmd_path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", cmd->args[0]);
        free_str_array(paths);
        exit(127); // Command not found
    }
    execve(cmd_path, cmd->args, mini->envp);
    // If execve returns, it failed
    perror("minishell: execve");
    free(cmd_path);
    free_str_array(paths);
    exit(126); // Command not executable
}

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
    // Close heredoc_fd in parent (child already duplicated or ignored it)
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1; // Mark as closed
    }
}


// Waits for all child processes and updates last_exit_status.
static void wait_for_children(pid_t *child_pids, int num_commands, t_struct *mini)
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
    if (current_cmd && current_cmd->next)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
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
        if (current_cmd->next && pipe(pipe_fds) == -1) // Create pipe if needed
        {
            perror("minishell: pipe");
            mini->last_exit_status = 1;
            free(child_pids);
            return (1);
        }

        pid = fork();
        if (pid == -1)
        {
            handle_fork_error(prev_pipe_in_fd, pipe_fds, current_cmd, child_pids, mini);
            return (1);
        }
        else if (pid == 0) // Child process
        {
            setup_child_pipes(current_cmd, pipe_fds, prev_pipe_in_fd);
            child_execute_command(current_cmd, mini); // This function handles execution and exits
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

// Función para ejecutar comandos
int execute_commands(t_command *commands, char **envp, int *last_exit_status_ptr)
{
    t_command *current_cmd;
    pid_t pid;
    int pipe_fds[2];
    int prev_pipe_in_fd = -1;
    pid_t *child_pids;
    int num_commands = 0;
    int i;

    current_cmd = commands;
    while (current_cmd)
    {
        num_commands++;
        current_cmd = current_cmd->next;
    }

    child_pids = ft_calloc(num_commands + 1, sizeof(pid_t));
    if (!child_pids)
    {
        perror("minishell: calloc");
        *last_exit_status_ptr = 1;
        // Los heredoc_fd abiertos son gestionados por parse_input al cerrar los previos
        // y por el padre después del fork.
        return (1);
    }

    num_commands = 0; // Reiniciar para usar como índice de child_pids
    current_cmd = commands;
    while (current_cmd)
    {
        // En Bash, un warning de heredoc no cambia el exit status.
        // La bandera `heredoc_error` se usa para que `handle_redirecctions_in_child`
        // sepa que no debe usar un `heredoc_fd` inválido para la entrada estándar.
        // No es necesario modificar el exit status del shell aquí solo por el warning.

        if (current_cmd->next) // Si no es el último comando en una tubería
        {
            if (pipe(pipe_fds) == -1)
            {
                perror("minishell: pipe");
                *last_exit_status_ptr = 1;
                if (prev_pipe_in_fd != -1)
                    close(prev_pipe_in_fd);
                free(child_pids);
                return (1);
            }
        }

        pid = fork();
        if (pid == -1)
        {
            perror("minishell: fork");
            *last_exit_status_ptr = 1;
            if (prev_pipe_in_fd != -1)
                close(prev_pipe_in_fd);
            if (current_cmd->next)
            {
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }
            // El heredoc_fd del comando actual se cerrará en el padre más abajo
            // o ya fue cerrado en parse_input si fue sobrescrito.
            free(child_pids);
            return (1);
        }
        else if (pid == 0) // Proceso hijo
        {
            // Configurar entrada de la tubería previa
            if (prev_pipe_in_fd != -1)
            {
                if (dup2(prev_pipe_in_fd, STDIN_FILENO) == -1)
                {
                    perror("minishell: dup2 stdin child");
                    exit(1);
                }
                close(prev_pipe_in_fd); // Cerrar el FD del pipe que se duplicó
            }

            // Configurar salida a la siguiente tubería (si existe)
            if (current_cmd->next)
            {
                if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
                {
                    perror("minishell: dup2 stdout child");
                    exit(1);
                }
            }

            // Cerrar los extremos del pipe que no se usarán en el hijo
            if (current_cmd->next)
            {
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }

            // Gestionar todas las redirecciones (incluido el heredoc)
            // handle_redirecctions_in_child usará cmd->heredoc_fd solo si heredoc_error es 0
            if (handle_redirecctions_in_child(current_cmd) != 0)
            {
                exit(1); // Error durante el manejo de redirecciones
            }

            if (current_cmd->args && current_cmd->args[0])
            {
                char **paths = get_paths(envp);
                char *cmd_path = find_cmd_path(current_cmd->args[0], paths);

                if (!cmd_path)
                {
                    fprintf(stderr, "minishell: %s: command not found\n", current_cmd->args[0]);
                    free_str_array(paths);
                    exit(127); // Command not found
                }
                execve(cmd_path, current_cmd->args, envp);
                perror("minishell: execve"); // Esto solo se ejecuta si execve falla
                free(cmd_path);
                free_str_array(paths);
                exit(126); // Command not executable
            }
            else // Caso de comando vacío (ej. solo redirecciones sin comando explícito)
            {
                exit(0);
            }
        }
        else // Proceso padre
        {
            child_pids[num_commands++] = pid; // Guardar el PID del hijo
            if (prev_pipe_in_fd != -1)
                close(prev_pipe_in_fd); // Cerrar el extremo de lectura del pipe anterior
            if (current_cmd->next)
            {
                close(pipe_fds[1]); // Cerrar el extremo de escritura del pipe actual
                prev_pipe_in_fd = pipe_fds[0]; // El extremo de lectura se convierte en la entrada para el siguiente comando
            }
            else // Es el último comando en la tubería
            {
                if (prev_pipe_in_fd != -1)
                {
                    close(prev_pipe_in_fd); // Asegurarse de cerrar el último pipe de entrada
                    prev_pipe_in_fd = -1;
                }
            }

            // Muy importante: Si el comando actual tiene un heredoc_fd abierto,
            // el padre debe cerrarlo para evitar fugas de descriptores de archivo.
            // El hijo ya lo habrá duplicado si era necesario.
            if (current_cmd->heredoc_fd != -1)
            {
                close(current_cmd->heredoc_fd);
                current_cmd->heredoc_fd = -1; // Marcar como cerrado
            }
        }
        current_cmd = current_cmd->next; // Pasar al siguiente comando
    }

    // Esperar a que todos los procesos hijos terminen
    int status;
    i = 0;
    while (i < num_commands)
    {
        waitpid(child_pids[i], &status, 0);
        if (i == num_commands - 1) // El estado de salida del último comando en la tubería es el que importa
        {
            if (WIFEXITED(status))
                *last_exit_status_ptr = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                *last_exit_status_ptr = 128 + WTERMSIG(status);
        }
        i++;
    }
    free(child_pids); // Liberar el array de PIDs
    return (0);
}