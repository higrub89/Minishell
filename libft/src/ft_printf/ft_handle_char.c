/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handle_char.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 20:04:20 by rhiguita          #+#    #+#             */
/*   Updated: 2025/01/27 20:04:27 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ft_printf.h"

int	ft_handle_char(va_list ap)
{
	char	c;

	c = va_arg(ap, int);
	return (write(1, &c, 1));
}
