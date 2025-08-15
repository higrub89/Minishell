#include "../inc/main_utils.h" // Incluye main_utils.h, que a su vez incluye minishell.h
#include "../inc/env_utils.h"  // Necesario para ft_copy_str_array y free_str_array

/**
 * @brief Inicializa la estructura principal de la minishell.
 * Realiza una copia del entorno original y establece el estado de salida inicial.
 * @param mini Puntero a la estructura t_minishell a inicializar.
 * @param envp_main El entorno recibido de la función main (char **envp).
 * @return 0 en éxito, 1 en caso de error de asignación de memoria.
 */
int init_minishell(t_struct *mini, char **envp_main)
{
    // Usa ft_copy_str_array de env_utils.c para crear una copia mutable del entorno.
    mini->envp = ft_copy_str_array(envp_main);
    if (!mini->envp)
    {
        perror("minishell: failed to copy environment");
        return (1); // Indica fallo de inicialización
    }
    mini->last_exit_status = 0; // Estado de salida inicial por defecto
    return (0); // Indica éxito
}

/**
 * @brief Limpia y libera toda la memoria asignada por la minishell antes de salir.
 * Libera la copia del entorno mantenida por la minishell.
 * @param mini Puntero a la estructura t_minishell a limpiar.
 */
void cleanup_minishell(t_struct *mini)
{
    if (mini && mini->envp) // Asegúrate de que mini y envp existan antes de intentar liberar
    {
        free_str_array(mini->envp); // Usa free_str_array de env_utils.c
        mini->envp = NULL; // Establecer a NULL previene doble liberación accidental
    }
    // Si la estructura t_minishell contuviera otros punteros asignados dinámicamente,
    // deberían ser liberados aquí también.
}

/**
 * @brief Verifica si una cadena contiene solo caracteres de espacio en blanco.
 * Utiliza ft_isspace de tu libft (o la estándar isspace).
 * @param s La cadena a verificar.
 * @return 1 si la cadena es nula, vacía o solo contiene espacios en blanco, 0 en caso contrario.
 */
int ft_str_is_whitespace(const char *s)
{
    if (!s) // Una cadena nula podría considerarse "vacía" para este propósito.
        return (1);
    while (*s)
    {
        if (!ft_isspace(*s)) // `ft_isspace` debe estar en tu libft
            return (0); // Encontrado un carácter que no es espacio en blanco
        s++;
    }
    return (1); // Todos los caracteres eran espacios en blanco o la cadena estaba vacía
}