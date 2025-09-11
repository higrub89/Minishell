/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:40:53 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/04 01:40:45 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

static int	count_args(char **args);
static void	print_exit_error(char *arg);

int			ft_exit(t_struct *mini, char **args);

static void	print_exit_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
}

static int	count_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
		i++;
	return (i);
}

static int	handle_exit_args(t_struct *mini, char **args)
{
	int	num_args;

	num_args = count_args(args);
	if (num_args > 2)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		mini->last_exit_status = 1;
		mini->should_exit = false;
		return (1);
	}
	if (num_args == 1)
		return (mini->last_exit_status);
	if (!is_numeric(args[1]) || will_overflow(args[1]))
	{
		print_exit_error(args[1]);
		return (255);
	}
	return ((unsigned char)safe_atoll(args[1]));
}

int	ft_exit(t_struct *mini, char **args)
{
	int	status;

	status = 0;
	if (!mini->is_piped)
		ft_putendl_fd("exit", STDOUT_FILENO);
	mini->should_exit = true;
	status = handle_exit_args(mini, args);
	mini->last_exit_status = status;
	if (mini->should_exit == false)
	{
		return (status);
	}
	if (mini->is_piped)
		exit(status);
	return (status);
}
