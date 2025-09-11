/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:45:13 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 11:00:36 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

char		*get_env_value(const char *name, t_struct *mini);
static int	update_existing_var(t_struct *mini, const char *key,
				const char	*new_var_str, size_t key_len);
static int	add_new_var(t_struct *mini, const char *new_var_str);
int			ft_setenv_var(t_struct *mini, const char *key, const char *value);

char	*get_env_value(const char *name, t_struct *mini)
{
	int		i;
	size_t	len;
	char	*str_status;

	if (!name || !mini || !mini->envp)
		return (ft_strdup(""));
	if (ft_strncmp(name, "?", 2) == 0)
	{
		str_status = ft_itoa(mini->last_exit_status);
		if (!str_status)
			return (ft_strdup(""));
		return (str_status);
	}
	len = ft_strlen(name);
	i = 0;
	while (mini->envp[i])
	{
		if (ft_strncmp(mini->envp[i], name, len) == 0
			&& mini->envp[i][len] == '=')
			return (ft_strdup(mini->envp[i] + len + 1));
		i++;
	}
	return (ft_strdup(""));
}

static int	update_existing_var(t_struct *mini, const char *key,
		const char *new_var_str, size_t key_len)
{
	int	i;

	i = 0;
	while (mini->envp[i])
	{
		if (ft_strncmp(mini->envp[i], key, key_len) == 0
			&& mini->envp[i][key_len] == '=')
		{
			free(mini->envp[i]);
			mini->envp[i] = (char *)new_var_str;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	add_new_var(t_struct *mini, const char *new_var_str)
{
	char	**new_envp;
	int		count;
	int		i;

	count = 0;
	while (mini->envp[count])
		count++;
	new_envp = (char **)malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
	{
		free((char *)new_var_str);
		return (1);
	}
	i = 0;
	while (mini->envp[i])
	{
		new_envp[i] = mini->envp[i];
		i++;
	}
	new_envp[i] = (char *)new_var_str;
	new_envp[i + 1] = NULL;
	free(mini->envp);
	mini->envp = new_envp;
	return (0);
}

int	ft_setenv_var(t_struct *mini, const char *key, const char *value)
{
	char	*new_var_str;
	char	*eq_key;
	size_t	key_len;

	key_len = ft_strlen(key);
	eq_key = ft_strjoin(key, "=");
	if (!eq_key)
		return (1);
	new_var_str = ft_strjoin(eq_key, value);
	free(eq_key);
	if (!new_var_str)
		return (1);
	if (update_existing_var(mini, key, new_var_str, key_len))
		return (0);
	else
		return (add_new_var(mini, new_var_str));
}
