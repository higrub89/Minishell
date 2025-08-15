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


static long long	safe_atoll(const char *str)
{
	int			sign;
	long long	result;
	size_t		i;

	sign = 1;
	result = 0;
	i = 0;

	// Saltar espacios iniciales
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;

	// Signo opcional
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}

	// Convertir dígitos manualmente
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}

	return (result * sign);
}

/**
 * @brief Implementación del builtin 'exit'.
 * Termina la minishell con un estado de salida específico.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos (args[0] es "exit", args[1] es el estado de salida opcional).
 * @return Nunca debería retornar, ya que llama a exit().
 */


int	ft_exit(t_struct *mini, char **args)
{
	long long	exit_code;
	int			num_args;

	ft_putendl_fd("exit", STDOUT_FILENO);

	num_args = 0;
	while (args[num_args])
		num_args++;

	if (num_args == 1)
		exit(mini->last_exit_status);
	else if (num_args >= 2)
	{
		if (!is_numeric(args[1]))
		{
			ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
			ft_putstr_fd(args[1], STDERR_FILENO);
			ft_putendl_fd(": numeric argument required", STDERR_FILENO);
			exit(255);
		}
		else if (num_args > 2)
		{
			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
			mini->last_exit_status = 1;
			return (1);
		}
		else
		{
			exit_code = safe_atoll(args[1]);
			exit((unsigned char)exit_code);
		}
	}
	return (0);
}