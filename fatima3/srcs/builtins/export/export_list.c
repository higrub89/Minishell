/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:51:59 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:01:28 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int			is_in_export_list(char **export_list, char *var);
char		**add_to_export_list(char **export_list, char *var);
static int	build_filtered_list(t_struct *mini, char *var_name,
				char **new_list);
char		**remove_from_export_list(t_struct *mini, char *var_name);

int	is_in_export_list(char **export_list, char *var)
{
	int	i;

	i = 0;
	while (export_list && export_list[i])
	{
		if (is_same_var_name(export_list[i], var))
			return (1);
		i++;
	}
	return (0);
}

char	**add_to_export_list(char **export_list, char *var)
{
	int		i;
	int		j;
	char	**new_list;

	i = 0;
	j = 0;
	while (export_list && export_list[i])
		i++;
	new_list = malloc(sizeof(char *) * (i + 2));
	if (!new_list)
		return (export_list);
	while (j < i)
	{
		new_list[j] = ft_strdup(export_list[j]);
		if (!new_list[j])
		{
			free_str_array(new_list);
			return (free_str_array(export_list), NULL);
		}
		j++;
	}
	new_list[i] = ft_strdup(var);
	new_list[i + 1] = NULL;
	free_str_array(export_list);
	return (new_list);
}

static int	build_filtered_list(t_struct *mini, char *var_name, char **new_list)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (mini->export_list[i])
	{
		if (!is_same_var_name(mini->export_list[i], var_name))
		{
			new_list[j] = ft_strdup(mini->export_list[i]);
			if (!new_list[j])
			{
				free_str_array(new_list);
				return (0);
			}
			j++;
		}
		i++;
	}
	new_list[j] = NULL;
	return (1);
}

char	**remove_from_export_list(t_struct *mini, char *var_name)
{
	int		count;
	char	**new_list;

	count = 0;
	if (!mini->export_list || !var_name)
		return (mini->export_list);
	while (mini->export_list[count])
		count++;
	new_list = malloc(sizeof(char *) * (count + 1));
	if (!new_list)
		return (mini->last_exit_status = 1, mini->export_list);
	if (!build_filtered_list(mini, var_name, new_list))
	{
		mini->last_exit_status = 1;
		return (mini->export_list);
	}
	free_str_array(mini->export_list);
	return (new_list);
}
