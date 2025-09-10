/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 23:03:12 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 23:03:15 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_UTILS_H
# define MAIN_UTILS_H

# include "minishell.h"

void	manage_shlvl(t_struct *mini);
void	handle_underscore(t_struct *mini, char *arg);
void	cleanup_minishell(t_struct *mini);
void	clean_and_exit(t_struct *mini, int status);
int		ft_str_is_whitespace(const char *s);

#endif
