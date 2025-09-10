/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:37:30 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:01:04 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

void		add_export_only(char **result, char **envp, char **export, int *i);
static void	export_error(char *arg, char *msg, int *status);
static void	export_with_value(t_struct *mini, char *arg);
static void	export_without_value(t_struct *mini, char *arg);
int			ft_export(t_struct *mini, char **arg);

void	add_export_only(char **result, char **envp, char **export, int *i)
{
	int	j;

	j = 0;
	while (export && export[j])
	{
		if (is_in_envp(envp, export[j]) == 0)
		{
			result[*i] = ft_strdup(export[j]);
			(*i)++;
		}
		j++;
	}
}

static void	export_error(char *arg, char *msg, int *status)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("`: ", STDERR_FILENO);
	ft_putendl_fd(msg, STDERR_FILENO);
	*status = 1;
}

static void	export_with_value(t_struct *mini, char *arg)
{
	char	*var_name;
	int		name_len;

	name_len = 0;
	while (arg[name_len] && arg[name_len] != '=')
		name_len++;
	var_name = ft_substr(arg, 0, name_len);
	if (is_in_export_list(mini->export_list, var_name))
		mini->export_list = remove_from_export_list(
				mini, var_name);
	mini->export_list = add_to_export_list(mini->export_list, arg);
	add_to_envp(mini, arg);
	free(var_name);
}

static void	export_without_value(t_struct *mini, char *arg)
{
	if (!is_in_export_list(mini->export_list, arg))
		mini->export_list = add_to_export_list(mini->export_list, arg);
}

int	ft_export(t_struct *mini, char **arg)
{
	int	i;
	int	status;

	i = 1;
	status = 0;
	if (!arg[1])
		return (print_export_env(mini), 0);
	while (arg[i])
	{
		if (arg[i][0] == '-')
			export_error(arg[i], "options are not supported", &status);
		else if (!is_valid_var_name(arg[i]))
			export_error(arg[i], "not a valid identifier", &status);
		else if (ft_strchr(arg[i], '='))
			export_with_value(mini, arg[i]);
		else
			export_without_value(mini, arg[i]);
		i++;
	}
	mini->last_exit_status = status;
	return (status);
}
