#include "../inc/minishell.h" // Incluye el master header que ya trae todo lo necesario



int main(int argc, char **argv, char **envp_main)
{
    char        *input_line;
    t_token     *token_list;
    t_command   *commands;
    t_struct mini; // ¡Instancia de la estructura principal de la minishell!

    (void)argc; // Para evitar warnings de unused parameter si no los usas
    (void)argv; // Para evitar warnings de unused parameter

    // 1. Inicializar la estructura principal de la minishell
    if (init_minishell(&mini, envp_main) != 0)
    {
        // Si falla la copia del entorno o cualquier otra inicialización, salir.
        return (1); 
    }

    // 2. Bucle principal de la shell: lectura, análisis, ejecución
  
    while (1)
    {
        set_signals(INTERACTIVE); // Configurar el manejador de señales para el modo interactivo
        input_line = readline("minishell> "); // Mostrar prompt y leer entrada

        if (!input_line) // Manejo de CTRL + D (EOF)
        {
            ft_putendl_fd("exit", STDOUT_FILENO); // Imprime "exit" como Bash
            break ; // Salir del bucle principal
        }
        
        // Si la línea no está vacía o solo contiene espacios en blanco, añadir al historial
        if (ft_strlen(input_line) > 0 && !ft_str_is_whitespace(input_line))
            add_history(input_line);
        
        // 3. Lexer: Convertir la línea de entrada en una lista de tokens
        // El lexer también puede necesitar 'mini' para actualizar el estado de salida en errores de sintaxis.
        token_list = lexer(input_line, &mini);
        if (!token_list) // Si el lexer devuelve NULL (error de sintaxis o línea vacía/comentarios)
        {
            free(input_line); // Liberar la línea leída
            // El estado de salida ya ha sido actualizado por lexer en caso de error.
            continue ; // Volver al inicio del bucle para pedir nueva entrada
        }
        
        // 4. Parser: Convertir la lista de tokens en una lista de comandos
        // El parser necesita 'mini' para actualizar el estado de salida en errores de sintaxis.
        commands = parse_input(token_list, &mini); 
        // ¡Importante liberar la lista de tokens después de que el parser los haya procesado!
        free_tokens(token_list); 

        if (!commands) // Si el parser devuelve NULL (error de sintaxis)
        {
            free(input_line); // Liberar la línea original
            // El estado de salida ya ha sido actualizado por parser en caso de error.
            continue ; // Volver al inicio del bucle
        }
        
        // 5. Expander: Expandir variables de entorno y tildes en los argumentos y redirecciones
        // El expander necesita 'mini' para acceder a 'mini->envp' y 'mini->last_exit_status'.
        if (process_heredoc_input(commands, &mini))
        {
            // Si hubo un error al procesar heredocs, se actualiza el estado de salida en process_heredoc_input
            free_commands(commands); // Liberar la lista de comandos antes de continuar
            free(input_line);        // Liberar la línea leída por readline
            continue ; // Volver al inicio del bucle
        }
        expand_variables(commands, &mini); 

        // 6. Executor: Ejecutar la lista de comandos (maneja pipes, redirecciones, builtins y externos)
        // El executor necesita 'mini' para manejar el entorno, el estado de salida, etc.
        execute_commands(commands, &mini); 
        
        // 7. Limpiar: Liberar la memoria asignada para los comandos y la línea de entrada
        free_commands(commands); // Libera la lista de comandos
        free(input_line);     
        if (mini.should_exit)
           break;
    }
    
    // 8. Limpieza final de la minishell antes de que el programa termine
    cleanup_minishell(&mini);
    
    // 9. Retornar el último estado de salida de la shell
    return (mini.last_exit_status); 
}