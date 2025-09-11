/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:41:45 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 09:43:41 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include <unistd.h> // For write

static void	interactive_handler(int signum)
{
	if (signum == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		get_mini()->last_exit_status = 130;
	}
}

static void	non_interactive_handler(int signum)
{
	(void)signum;
}

void	set_signals(t_mode mode)
{
	if (mode == INTERACTIVE)
	{
		signal(SIGINT, interactive_handler);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == NON_INTERACTIVE)
	{
		signal(SIGINT, non_interactive_handler);
		signal(SIGQUIT, non_interactive_handler);
	}
	else if (mode == CHILD)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
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
