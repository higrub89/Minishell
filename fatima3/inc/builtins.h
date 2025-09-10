/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 09:30:57 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 10:59:14 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "env_utils.h"
# include "executor.h"
# include "minishell.h"
# include <errno.h>
# include <limits.h>
# include <sys/stat.h>

// echo/builtin_echo.c
int			ft_echo(char **args);

// env/builtin_env.c
int			ft_env(t_struct *mini, char **args);

// env/env_utils.c
char		*get_env_value(const char *name, t_struct *mini);
int			ft_setenv_var(t_struct *mini, const char *key, const char *value);

// env/array_copy;
char		**ft_copy_str_array(char **arr);

// pwd/builtin_pwd.c
int			ft_pwd(t_struct *mini, char **args);

// unset/builtin_unset.c
int			ft_unset(t_struct *mini, char **args);

// cd/builtin_cd.c
int			ft_cd(t_struct *mini, char **args);

// cd/cd_special.c
int			go_home(t_struct *mini);
int			go_to_oldpwd(t_struct *mini);
int			go_to_parent(t_struct *mini);

// cd/cd_utils.c
int			is_valid_directory(const char *path);
int			update_pwd_after_chdir(t_struct *mini);
void		update_pwd_env(t_struct *mini, const char *new_path);

// exit/builtin_exit.c
int			ft_exit(t_struct *mini, char **args);

// exit/exit_utils.c
int			is_numeric(char *str);
int			will_overflow(const char *str);
long long	safe_atoll(const char *str);

// export/builtin_export.c
void		add_export_only(char **result, char **envp, char **export, int *i);
int			ft_export(t_struct *mini, char **args);

// export/export_display.c
void		init_export_list(t_struct *mini);
void		print_export_line(char *entry);
char		**merge_env_and_export(char **envp, char **export);
void		print_export_env(t_struct *mini);

// export/export_envp.c
int			is_in_envp(char **envp, char *var);
void		add_to_envp(t_struct *mini, char *arg);

// export/export_list.c
int			is_in_export_list(char **export_list, char *var);
char		**add_to_export_list(char **export_list, char *var);
char		**remove_from_export_list(t_struct *mini, char *var_name);

// export/export_utils.c
int			is_valid_var_name(char *argv);
int			is_same_var_name(const char *entry, const char *var);
int			count_str_array(char **array);
int			count_export_only(char **envp, char **export);
void		sort_envp(char **envp);

#endif
