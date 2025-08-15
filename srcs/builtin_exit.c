#include "../inc/builtins.h"

/**
 * @brief Verifica si una cadena es un número válido para el estado de salida.
 * Bash permite números que pueden ser negativos o muy grandes, y los trunca a 8 bits.
 * @param str La cadena a verificar.
 * @return 1 si es un número válido, 0 si no.
 */
static int is_numeric(char *str)
{
    int i = 0;
    if (!str || str[0] == '\0')
        return (0);
    // Permitir signos '+' o '-' al principio
    if (str[0] == '-' || str[0] == '+')
        i++;
    // Verificar que el resto de caracteres sean dígitos
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
            return (0); // No es un dígito
        i++;
    }
    // Si la cadena solo contenía el signo, tampoco es un número válido ("-", "+")
    if (i == 1 && (str[0] == '-' || str[0] == '+'))
        return (0);
    return (1);
}

/**
 * @brief Implementación del builtin 'exit'.
 * Termina la minishell con un estado de salida específico.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos (args[0] es "exit", args[1] es el estado de salida opcional).
 * @return Nunca debería retornar, ya que llama a exit().
 */
int ft_exit(t_struct *mini, char **args)
{
    long long   exit_code; // Usar long long para manejar valores grandes
    int         num_args;

    // Bash siempre imprime "exit" en la salida estándar cuando se invoca.
    ft_putendl_fd("exit", STDOUT_FILENO);

    num_args = 0;
    while (args[num_args])
        num_args++;

    // Caso 1: 'exit' sin argumentos (num_args == 1)
    if (num_args == 1)
    {
        // Sale con el último estado de salida de la minishell
        exit(mini->last_exit_status);
    }
    // Caso 2: 'exit <argumento>'
    else if (num_args >= 2)
    {
        if (!is_numeric(args[1]))
        {
            // Error: argumento no numérico (ej. 'exit abc')
            ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
            ft_putstr_fd(args[1], STDERR_FILENO);
            ft_putendl_fd(": numeric argument required", STDERR_FILENO);
            exit(255); // Estado de salida 255 para error de argumento no numérico
        }
        else if (num_args > 2)
        {
            // Error: demasiados argumentos si el primero es numérico (ej. 'exit 1 2 3')
            ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
            // Bash establece el estado de salida a 1 y NO sale de la shell.
            mini->last_exit_status = 1;
            return (1); // Retorna para que la shell continúe (como Bash)
        }
        else // 'exit <código_válido>' (num_args == 2 y args[1] es numérico)
        {
            // Convierte el argumento a un número.
            // ft_atol es tu propia función atoi/atoll para long long.
            exit_code = ft_atol(args[1]); 
            // Bash trunca el estado de salida a un byte (0-255).
            // Un valor negativo se convierte a un número positivo en ese rango.
            // Ej: exit -1 -> 255, exit 256 -> 0.
            exit((unsigned char)exit_code);
        }
    }
    // Este retorno es solo para satisfacer al compilador, ya que exit() terminará el proceso.
    return (0); 
}