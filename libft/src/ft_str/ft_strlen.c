/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 20:14:01 by rhiguita          #+#    #+#             */
/*   Updated: 2025/01/27 20:14:03 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

size_t	ft_strlen(const char *s)
{
	const char	*start;

	start = s;
	while (*s++)
		;
	return (s - start - 1);
}
