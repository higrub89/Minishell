#include "../../inc/builtins.h" // Incluye el header de builtins

/**
 * @brief Implementación del builtin 'pwd'.
 * Imprime el directorio de trabajo actual en la salida estándar (STDOUT_FILENO).
 * Utiliza getcwd para obtener la ruta canónica del directorio.
 * @param mini Puntero a la estructura principal de la minishell.
 * @return 0 en caso de éxito, 1 en caso de error (ej. getcwd falla).
 */
int ft_pwd(t_struct *mini)
{
    char    *cwd;

    // getcwd(NULL, 0) le permite a la función asignar dinámicamente la memoria
    // necesaria para almacenar la ruta, lo que la hace muy robusta.
    cwd = getcwd(NULL, 0); 
    if (!cwd)
    {
        // Si getcwd falla (por ejemplo, directorio inaccesible o error de memoria),
        // perror imprimirá un mensaje de error descriptivo basado en errno.
        perror("minishell: pwd"); 
        mini->last_exit_status = 1; // Establece el estado de salida a 1 (error)
        return (1);
    }
    
    // Imprime la ruta obtenida seguida de un salto de línea.
    ft_putendl_fd(cwd, STDOUT_FILENO); 
    free(cwd); // Libera la memoria asignada por getcwd
    
    mini->last_exit_status = 0; // Establece el estado de salida a 0 (éxito)
    return (0);
}