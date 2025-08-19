#include "../inc/minishell.h"

/**
 * @brief Manejador para el modo INTERACTIVO (leyendo en el prompt).
 * Muestra una nueva línea y refresca el prompt de readline.
 */
static void	interactive_handler(int signum)
{
	// Este manejador está perfecto, no se toca.
	if (signum == SIGINT)
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * @brief Configura los manejadores de señales según el modo de la shell.
 * Esta es la función central que replica la lógica de `mini_git`.
 * @param mode El estado actual: INTERACTIVE, NON_INTERACTIVE, o CHILD.
 */
void	set_signals(t_mode mode)
{
	if (mode == INTERACTIVE)
	{
		// Modo prompt: Ctrl+C llama a nuestro manejador, Ctrl+\ se ignora.
		signal(SIGINT, interactive_handler);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == NON_INTERACTIVE)
	{
		// Modo ejecución (Padre): El padre ignora ambas señales
		// para que solo afecten al hijo. ESTE ES EL CAMBIO CLAVE.
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == CHILD)
	{
		// Modo ejecución (Hijo): El hijo usa el comportamiento por defecto
		// del sistema operativo (terminar el proceso).
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
}