/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 20:10:47 by rhiguita          #+#    #+#             */
/*   Updated: 2025/01/27 20:10:49 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t buffsize)
{
	size_t	dstlen;
	size_t	srclen;

	dstlen = ft_strlen(dst);
	srclen = ft_strlen(src);
	if (dstlen < buffsize - 1 && buffsize > 0)
		ft_strlcpy(&dst[dstlen], src, buffsize - dstlen);
	if (dstlen >= buffsize)
		dstlen = buffsize;
	return (dstlen + srclen);
}
