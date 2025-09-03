#include "../inc/builtins.h"

//Solución: Cambia el valor de mini->last_exit_status = 255; por 2. Verificar quiza varía según ordenador.
//overflow se verifica en will_overflow.
//Así que si las variables persisten, el problema podría estar en cómo las gestionas internamente (por ejemplo, si las guardas en una estructura global y no las liberas correctamente al salir).
//Causa: is_numeric() no permite espacios, pero Bash sí los ignora

static int  is_numeric(char *str)
{
	int	i;
	int	digits;

	i = 0;
	if (!str)
		return (0);
	// Saltar espacios iniciales - para el error exit " 8"
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '\0')
		return (0);
	// Signo opcional
	if (str[i] == '+' || str[i] == '-')
		i++;
	digits = 0;
	while (str[i])
	{
		if (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		{
			i++;
			continue;
		}
		if (!ft_isdigit(str[i]))
			return (0);
		digits++;
		i++;
	}
	return (digits > 0);
}

static int	will_overflow(const char *str)
{
	int 				i;
	int 				sign;
	unsigned long long	result;
	int 				digits;

	i = 0;
	sign = 1;
	result = 0;
	// Saltar espacios
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	digits = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		digits++;
		if ((sign == 1 && result > LLONG_MAX) ||
			(sign == -1 && result > (unsigned long long)LLONG_MAX + 1))
				return (1); // Overflow
		i++;
	}
	return (0); // No overflow
}

static long long    safe_atoll(const char *str)
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

int	ft_exit(t_struct *mini, char **args)
{
	long long	exit_code;
	int			num_args;

	num_args = 0;
	ft_putendl_fd("exit", STDOUT_FILENO);
	while (args[num_args])
		num_args++;
	if (num_args == 1)
	{
		mini->should_exit = true;
		return mini->last_exit_status;
	}
	if (!is_numeric(args[1]) || will_overflow(args[1]))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		mini->last_exit_status = 255;
		mini->should_exit = true;
	}
	else if (num_args > 2)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		mini->last_exit_status = 1;
		mini->should_exit = false;
	}
	else
	{
		exit_code = safe_atoll(args[1]);
		mini->last_exit_status = (unsigned char)exit_code;
		mini->should_exit = true;
	}
	return mini->last_exit_status;
}
