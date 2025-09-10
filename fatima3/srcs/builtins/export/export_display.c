/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_display.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:51:34 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/07 11:38:22 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

void		init_export_list(t_struct *mini);
void		print_export_line(char *entry);
static void	print_env_filtered(char **copy_env);
char		**merge_env_and_export(char **envp, char **export);
void		print_export_env(t_struct *mini);

void	init_export_list(t_struct *mini)
{
	if (mini->envp)
		mini->export_list = ft_copy_str_array(mini->envp);
	else
	{
		mini->export_list = malloc(sizeof(char *));
		if (!mini->export_list)
		{
			mini->last_exit_status = 1;
			return ;
		}
		mini->export_list[0] = NULL;
	}
}

void	print_export_line(char *entry)
{
	char	*equal_sign;

	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	equal_sign = ft_strchr(entry, '=');
	if (equal_sign)
	{
		write(STDOUT_FILENO, entry, equal_sign - entry);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
		ft_putendl_fd("\"", STDOUT_FILENO);
	}
	else
		ft_putendl_fd(entry, STDOUT_FILENO);
}

static void	print_env_filtered(char **copy_env)
{
	int	i;

	i = 0;
	while (copy_env[i])
	{
		if (ft_strncmp(copy_env[i], "_=", 2) != 0)
			print_export_line(copy_env[i]);
		i++;
	}
}

char	**merge_env_and_export(char **envp, char **export)
{
	char	**result;
	int		env_count;
	int		export_only_count;
	int		i;

	env_count = count_str_array(envp);
	export_only_count = count_export_only(envp, export);
	result = malloc(sizeof(char *) * (env_count + export_only_count + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (envp && envp[i])
	{
		result[i] = ft_strdup(envp[i]);
		i++;
	}
	add_export_only(result, envp, export, &i);
	result[i] = NULL;
	return (result);
}

void	print_export_env(t_struct *mini)
{
	char	**merged;

	if (!mini->export_list)
		init_export_list(mini);
	merged = merge_env_and_export(mini->envp, mini->export_list);
	if (!merged)
	{
		mini->last_exit_status = 1;
		return ;
	}
	sort_envp(merged);
	print_env_filtered(merged);
	free_str_array(merged);
	mini->last_exit_status = 0;
}
