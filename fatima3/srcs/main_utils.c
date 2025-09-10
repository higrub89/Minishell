/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:39:23 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 07:39:26 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/main_utils.h"
#include "../inc/env_utils.h"
#include "../inc/builtins.h"

static int	is_all_digits(const char *s)
{
	if (!s || !*s)
		return (0);
	while (*s)
	{
		if (!ft_isdigit(*s))
			return (0);
		s++;
	}
	return (1);
}

void	manage_shlvl(t_struct *mini)
{
	char	*current_shlvl_str;
	int		shlvl_num;
	char	*new_shlvl_val;

	current_shlvl_str = get_env_value("SHLVL", mini);
	shlvl_num = 1;
	if (current_shlvl_str && is_all_digits(current_shlvl_str))
	{
		shlvl_num = ft_atoi(current_shlvl_str) + 1;
		if (shlvl_num < 1)
			shlvl_num = 1;
	}
	free(current_shlvl_str);
	new_shlvl_val = ft_itoa(shlvl_num);
	if (new_shlvl_val)
	{
		ft_setenv_var(mini, "SHLVL", new_shlvl_val);
		free(new_shlvl_val);
	}
}

void	handle_underscore(t_struct *mini, char *arg) //agreagado toda la fx
{
	char	*underscore;
	char	*args[3];

	underscore = ft_strjoin("_=", arg);
	if (!underscore)
		return ;
	args[0] = "export";
	args[1] = underscore;
	args[2] = NULL;
	ft_export(mini, args);
	free(underscore);
}

void	cleanup_minishell(t_struct *mini)
{
	rl_clear_history();
	if (mini && mini->envp)
	{
		free_str_array(mini->envp);
		mini->envp = NULL;
	}
	if (mini && mini->export_list)
	{
		free_str_array(mini->export_list);
		mini->export_list = NULL;
	}
}

void	clean_and_exit(t_struct *mini, int status)
{
	cleanup_minishell(mini);
	exit(status);
}

int	ft_str_is_whitespace(const char *s)
{
	if (!s)
		return (1);
	while (*s)
	{
		if (!ft_isspace(*s))
			return (0);
		s++;
	}
	return (1);
}
