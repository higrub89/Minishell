#include "../inc/builtins.h" // Incluye el nuevo header de builtins, que a su vez incluye env_utils.h

/**
 * @brief Valida si el nombre de una variable es válido para unset.
 * Un nombre válido no puede ser una cadena vacía, no puede empezar con dígito,
 * y solo puede contener caracteres alfanuméricos y guiones bajos ('_').
 * @param argv Nombre de la variable a validar.
 * @return 1 si es válido, 0 si no.
 */
static int is_valid_var_name(char *argv)
{
    int i = 0;
    
    if (!argv || argv[0] == '\0') // Un nombre vacío no es válido
        return (0);
    
    if (ft_isdigit(argv[0])) // El primer carácter no puede ser un dígito
        return (0);
    
    while (argv[i])
    {
        // Solo permite caracteres alfanuméricos y guiones bajos
        if (!ft_isalnum(argv[i]) && argv[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

/**
 * @brief Elimina una variable del array de entorno (envp).
 * Desplaza los elementos posteriores para llenar el espacio, y libera la memoria
 * de la string de la variable eliminada.
 * @param envp El array de strings del entorno (mini->envp).
 * @param name_var El nombre de la variable a eliminar.
 */
static void remove_var_from_envp(char **envp, char *name_var)
{
    int i;
    int j;
    
    i = 0;
    while (envp[i])
    {
        j = 0;
        // Encuentra el largo del nombre de la variable en el entorno hasta '='
        while (envp[i][j] && envp[i][j] != '=')
            j++;
        
        // Compara el nombre (hasta el '=') con la variable del entorno
        if (ft_strncmp(envp[i], name_var, j) == 0 && envp[i][j] == '=')
        {
            free(envp[i]); // Libera la string de la variable a eliminar
            // Desplaza los punteros de las strings siguientes hacia arriba
            while (envp[i + 1])
            {
                envp[i] = envp[i + 1];
                i++;
            }
            envp[i] = NULL; // El último elemento se convierte en NULL
            return;
        }
        i++;
    }
    // Bash no muestra error si la variable no se encuentra, así que no se hace aquí.
}

/**
 * @brief Implementación del builtin 'unset'.
 * Elimina variables del entorno. Imprime un error en STDERR_FILENO para
 * nombres de variables inválidos y establece el estado de salida.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos (args[0] es "unset", args[1] en adelante son nombres de variables).
 * @return 0 en caso de éxito, 1 si al menos un nombre de variable fue inválido.
 */
int ft_unset(t_struct *mini, char **args)
{
    int i;
    int status; // 0 para éxito, 1 para error (el estado de salida de Bash)

    i = 1;
    status = 0; // Asume éxito inicialmente.

    while (args[i])
    {
        if (!is_valid_var_name(args[i]))
        {
            ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
            ft_putstr_fd(args[i], STDERR_FILENO);
            ft_putendl_fd("`: not a valid identifier", STDERR_FILENO);
            status = 1; // Marca que hubo un error, pero sigue procesando otros argumentos.
        }
        else
        {
            // Solo intenta remover si el nombre es válido.
            remove_var_from_envp(mini->envp, args[i]);
        }
        i++;
    }
    mini->last_exit_status = status; // Actualiza el estado de salida de la shell.
    return (status);
}