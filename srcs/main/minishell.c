/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 07:41:19 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/04 00:11:25 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"
#include "../inc/lexer.h"
#include "../inc/main_utils.h"
#include "../inc/minishell.h"
#include "../inc/parser.h"

int	process_input(char *input, t_token **tokens, t_command **cmds,
		t_struct *mini)
{
	*tokens = lexer(input, mini);
	if (!*tokens)
	{
		return (0);
	}
	*cmds = parse_input(*tokens, mini);
	free_tokens(*tokens);
	if (!*cmds)
	{
		return (0);
	}
	return (1);
}

int	handle_empty_env(t_struct *mini, char *arg)
{
	char	*cwd;
	char	*tmp;
	char	*args[3];

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: cd: getcwd failed");
		return (mini->last_exit_status = 1, 1);
	}
	tmp = ft_strjoin("PWD=", cwd);
	free(cwd);
	if (!tmp)
		return (mini->last_exit_status = 1, 1);
	args[0] = "export";
	args[1] = tmp;
	args[2] = NULL;
	ft_export(mini, args);
	free(tmp);
	manage_shlvl(mini);
	handle_underscore(mini, arg);
	return (0);
}

int	init_minishell(t_struct *mini, char **envp_main, char *arg)
{
	mini->envp = ft_copy_str_array(envp_main);
	mini->export_list = NULL;
	if (!mini->envp)
	{
		perror("minishell: failed to copy environment");
		return (1);
	}
	if (!mini->envp[0] && handle_empty_env(mini, arg))
		return (1);
	manage_shlvl(mini);
	mini->last_exit_status = 0;
	mini->should_exit = false;
	mini->child_interrupted_by_signal = false;
	return (0);
}

int	main(int argc, char **argv, char **envp_main)
{
	t_struct	*mini;

	mini = get_mini();
	(void)argc;
	(void)argv;
	if (init_minishell(mini, envp_main, argv[0]) != 0)
		return (1);
	run_minishell_loop(mini);
	cleanup_minishell(mini);
	return (mini->last_exit_status);
}
