/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_copy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:43:17 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 07:43:19 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/env_utils.h"

static void	free_on_error(char **arr, int i)
{
	while (--i >= 0)
		free(arr[i]);
	free(arr);
}

char	**ft_copy_str_array(char **arr)
{
	char	**new_arr;
	int		count;
	int		i;

	if (!arr)
		return (NULL);
	count = 0;
	while (arr[count])
		count++;
	new_arr = (char **)malloc(sizeof(char *) * (count + 1));
	if (!new_arr)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_arr[i] = ft_strdup(arr[i]);
		if (!new_arr[i])
		{
			free_on_error(new_arr, i);
			return (NULL);
		}
		i++;
	}
	new_arr[count] = NULL;
	return (new_arr);
}

void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
