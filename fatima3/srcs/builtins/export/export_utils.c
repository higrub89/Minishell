/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:52:16 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:01:36 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int			is_valid_var_name(char *argv);
int			is_same_var_name(const char *entry, const char *var);
int			count_str_array(char **array);
int			count_export_only(char **envp, char **export);
void		sort_envp(char **envp);

int	count_str_array(char **array)
{
	int	i;

	i = 0;
	while (array && array[i])
		i++;
	return (i);
}

int	count_export_only(char **envp, char **export)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (export && export[i])
	{
		if (is_in_envp(envp, export[i]) == 0)
			count++;
		i++;
	}
	return (count);
}

int	is_valid_var_name(char *argv)
{
	int	i;

	i = 0;
	if (!argv || argv[0] == '=' || ft_isdigit(argv[0]))
		return (0);
	while (argv[i] && argv[i] != '=')
	{
		if (!ft_isalnum(argv[i]) && argv[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_same_var_name(const char *entry, const char *var)
{
	int	i;

	i = 0;
	while (entry[i] && var[i] && entry[i] == var[i])
		i++;
	if (var[i] == '\0' && (entry[i] == '=' || entry[i] == '\0'))
		return (1);
	return (0);
}

void	sort_envp(char **envp)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (envp[i])
	{
		j = 0;
		while (envp[j + 1])
		{
			if (ft_strcmp(envp[j], envp[j + 1]) > 0)
			{
				tmp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}
