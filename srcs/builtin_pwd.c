#include "../inc/builtins.h" // Incluye el header de builtins

int ft_pwd(t_struct *mini)
{
    char    *cwd;

    // getcwd(NULL, 0) permite que el sistema asigne memoria automáticamente
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        // Fallback: buscar PWD en envp si getcwd falla
        int i = 0;
        while (mini->envp[i])
        {
            if (ft_strncmp(mini->envp[i], "PWD=", 4) == 0)
            {
                ft_putendl_fd(mini->envp[i] + 4, STDOUT_FILENO);
                mini->last_exit_status = 0;
                return (0);
            }
            i++;
        }
        // Si no se encuentra PWD, mostrar error
        perror("minishell: pwd");
        mini->last_exit_status = 1;
        return (1);
    }
    // Imprime el directorio actual y libera la memoria
    ft_putendl_fd(cwd, STDOUT_FILENO);
    free(cwd);
    mini->last_exit_status = 0;
    return (0);
}
