#include "../inc/builtins.h" // Incluye el header de builtins, que a su vez incluye env_utils.h

/**
 * @brief Implementación del builtin 'env'.
 * Imprime todas las variables de entorno activas en la salida estándar (STDOUT_FILENO).
 * Solo se imprimen las variables que contienen un signo de igual ('='), siguiendo
 * el comportamiento estándar de 'env' en shells como Bash.
 * @param mini Puntero a la estructura principal de la minishell.
 * @return 0 en caso de éxito.
 */
int ft_env(t_struct *mini)
{
    int i = 0;
    while (mini->envp[i])
    {
        // Solo imprime variables que contienen un '='.
        // Las variables exportadas sin valor (ej. `export MYVAR`) no se muestran con `env`,
        // pero sí con `export` sin argumentos.
        if (ft_strchr(mini->envp[i], '='))
            ft_putendl_fd(mini->envp[i], STDOUT_FILENO);
        i++;
    }
    // El comando 'env' siempre retorna 0 si se ejecuta sin errores de sintaxis,
    // incluso si el entorno está vacío.
    mini->last_exit_status = 0; 
    return (0);
}