/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 19:56:10 by rhiguita          #+#    #+#             */
/*   Updated: 2025/06/20 14:40:27 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>

int	ft_printf(const char *format, ...);
int	ft_handle_char(va_list ap);
int	ft_handle_string(va_list ap);
int	ft_handle_int(va_list ap);
int	ft_handle_unsigned(va_list ap);
int	ft_handle_pointer(va_list ap);
int	ft_handle_hexlower(va_list ap);
int	ft_handle_hexupper(va_list ap);
int	ft_handle_percent(void);
int	ft_putchar(char c);
int	ft_putnbr_base(unsigned long long nb, const char *base);

#endif
