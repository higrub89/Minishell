/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 08:03:58 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 08:04:00 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_UTILS_H
# define ENV_UTILS_H

# include "minishell.h"

char	**ft_copy_str_array(char **arr);
void	free_str_array(char **arr);
char	*get_env_value(const char *name, t_struct *mini);
int		ft_setenv_var(t_struct *mini, const char *key, const char *value);

#endif
