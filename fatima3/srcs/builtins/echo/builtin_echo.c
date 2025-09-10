/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:37:47 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:00:11 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int	is_valid_n_case(char **arg)
{
	int	i;
	int	j;

	i = 1;
	while (arg[i])
	{
		j = 0;
		if (arg[i][j] == '-')
		{
			j++;
			if (arg[i][j] != 'n')
				break ;
			while (arg[i][j] == 'n')
				j++;
			if (arg[i][j] != '\0')
				break ;
		}
		else
			break ;
		i++;
	}
	return (i);
}

int	ft_echo(char **arg)
{
	int	i;
	int	n_index;

	i = is_valid_n_case(arg);
	n_index = i;
	if (arg[i])
	{
		while (arg[i])
		{
			printf("%s", arg[i]);
			if (arg[i + 1])
				printf(" ");
			i++;
		}
		if (n_index == 1)
			printf("\n");
	}
	else if (n_index == 1)
		printf("\n");
	return (0);
}
