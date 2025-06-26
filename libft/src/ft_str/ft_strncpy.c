/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 20:14:41 by rhiguita          #+#    #+#             */
/*   Updated: 2025/01/27 20:14:42 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

size_t	ft_strncpy(char *dst, const char *src, size_t dstsize)
{
	size_t		len;
	size_t		i;

	len = 0;
	while (src[len] != '\0')
		len++;
	if (dstsize == 0)
		return (len);
	if (dst == NULL)
		return (len);
	i = 0;
	while (i < dstsize && src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	while (i < dstsize)
	{
		dst[i] = '\0';
		i++;
	}
	return (len);
}
