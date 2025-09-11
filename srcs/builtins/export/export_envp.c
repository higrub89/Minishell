/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_envp.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:51:48 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:01:20 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int			is_in_envp(char **envp, char *var);
void		add_to_envp(t_struct *mini, char *arg);
static int	match_env_var(char *env, char *arg, int len);
static void	update_env_var(t_struct *mini, int i, char *arg);
static char	**copy_envp_with_new(char **envp, char *arg, int count);

int	is_in_envp(char **envp, char *var)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (is_same_var_name(envp[i], var))
			return (1);
		i++;
	}
	return (0);
}

void	add_to_envp(t_struct *mini, char *arg)
{
	int		i;
	int		name_len;
	char	**new_envp;

	name_len = 0;
	while (arg[name_len] && arg[name_len] != '=')
		name_len++;
	i = 0;
	while (mini->envp && mini->envp[i])
	{
		if (match_env_var(mini->envp[i], arg, name_len))
			return (update_env_var(mini, i, arg));
		i++;
	}
	new_envp = copy_envp_with_new(mini->envp, arg, i);
	if (!new_envp)
		return ((void)(mini->last_exit_status = 1));
	free_str_array(mini->envp);
	mini->envp = new_envp;
}

static int	match_env_var(char *env, char *arg, int len)
{
	int	j;

	j = 0;
	while (j < len && env[j] == arg[j])
		j++;
	if (j == len)
	{
		if (env[j] == '=')
			return (1);
	}
	return (0);
}

static void	update_env_var(t_struct *mini, int i, char *arg)
{
	free(mini->envp[i]);
	mini->envp[i] = ft_strdup(arg);
}

static char	**copy_envp_with_new(char **envp, char *arg, int count)
{
	char	**new_envp;
	int		k;

	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (NULL);
	k = 0;
	while (k < count)
	{
		new_envp[k] = ft_strdup(envp[k]);
		k++;
	}
	new_envp[count] = ft_strdup(arg);
	new_envp[count + 1] = NULL;
	return (new_envp);
}
