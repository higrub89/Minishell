/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:51:05 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/04 00:13:23 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"
#include "../../../inc/lexer.h"

int			is_numeric(char *str);
int			will_overflow(const char *str);
long long	safe_atoll(const char *str);

int	is_numeric(char *str)
{
	int	i;
	int	digits;

	i = 0;
	if (!str)
		return (0);
	i = skip_spaces(str, i);
	if (str[i] == '\0')
		return (0);
	if (str[i] == '+' || str[i] == '-')
		i++;
	digits = 0;
	while (str[i])
	{
		if (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		{
			i++;
			continue ;
		}
		if (!ft_isdigit(str[i]))
			return (0);
		digits++;
		i++;
	}
	return (digits > 0);
}

int	will_overflow(const char *str)
{
	int					i;
	int					sign;
	unsigned long long	result;
	int					digits;

	i = 0;
	sign = 1;
	result = 0;
	i = skip_spaces(str, i);
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i++] == '-')
			sign = -1;
	}
	digits = 0;
	while (ft_isdigit(str[i]))
	{
		result = result * 10 + (str[i++] - '0');
		digits++;
		if ((sign == 1 && result > LLONG_MAX)
			|| (sign == -1 && result > (unsigned long long)LLONG_MAX + 1))
			return (1);
	}
	return (0);
}

long long	safe_atoll(const char *str)
{
	int			sign;
	long long	result;
	size_t		i;

	sign = 1;
	result = 0;
	i = 0;
	i = skip_spaces(str, i);
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}
