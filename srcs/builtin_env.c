#include "../inc/builtins.h" // Incluye el header de builtins, que a su vez incluye env_utils.h


int ft_env(t_struct *mini)
{
    int i = 0;

    while (mini->envp[i])
    {
        // Solo imprime variables que contienen '='
        if (ft_strchr(mini->envp[i], '='))
            ft_putendl_fd(mini->envp[i], STDOUT_FILENO);
        i++;
    }
    mini->last_exit_status = 0;
    return (0);
}