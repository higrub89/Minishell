#include "../inc/builtins.h" // Incluye el header de builtins

/**
 * @brief Verifica si una cadena corresponde a la opción '-n' para 'echo'.
 * La opción '-n' puede aparecer múltiples veces (ej. -n -n -nnnn) al inicio.
 * @param arg La cadena a verificar.
 * @return 1 si es una opción '-n' válida, 0 en caso contrario.
 */
static int is_n_option(char *arg)
{
    int i = 0;

    if (!arg || arg[0] != '-')
        return (0);
    i++; // Saltar el '-'
    if (arg[i] != 'n')
        return (0);
    while (arg[i] == 'n')
        i++;
    // Si la cadena solo contenía '-' y 'n's, es una opción '-n' válida.
    if (arg[i] == '\0')
        return (1);
    return (0);
}

/**
 * @brief Implementación del builtin 'echo'.
 * Imprime los argumentos en la salida estándar, con manejo de la opción '-n'.
 * @param args Array de argumentos (args[0] es "echo", seguido de los textos).
 * @return Siempre 0 (echo no suele fallar a menos que haya un problema de escritura).
 */
int ft_echo(char **args)
{
    int i = 1;         // Empezamos en el primer argumento (después de "echo")
    int print_newline = 1; // Por defecto, se imprime un salto de línea

    // 1. Procesar opciones '-n'
    // Los '-n' iniciales son opciones; cualquier otra cosa detiene el procesamiento de opciones.
    while (args[i] && is_n_option(args[i]))
    {
        print_newline = 0; // Si encontramos '-n', suprimimos el salto de línea
        i++; // Moverse al siguiente argumento
    }

    // 2. Imprimir los argumentos restantes
    while (args[i])
    {
        ft_putstr_fd(args[i], STDOUT_FILENO); // Imprimir el argumento
        if (args[i + 1]) // Si no es el último argumento, imprimir un espacio
            ft_putchar_fd(' ', STDOUT_FILENO);
        i++;
    }

    // 3. Imprimir el salto de línea si no se suprimió
    if (print_newline)
        ft_putchar_fd('\n', STDOUT_FILENO);
    
    // Echo siempre retorna 0 en éxito. No afecta last_exit_status de t_struct
    // porque no tiene acceso directo a 'mini'. Quien llame a ft_echo
    // debe establecer el estado de salida. En executor, por ejemplo,
    // puedes hacer mini->last_exit_status = ft_echo(cmd->args);
    return (0);
}