/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_builder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 22:53:43 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 22:53:44 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/expander.h"

void	ft_sb_init(t_string_builder *sb);
bool	ft_sb_grow(t_string_builder *sb, size_t needed);
void	ft_sb_append_char(t_string_builder *sb, char c);
void	ft_sb_append_str(t_string_builder *sb, const char *str);
char	*ft_sb_build(t_string_builder *sb);

void	ft_sb_init(t_string_builder *sb)
{
	sb->length = 0;
	sb->capacity = INITIAL_SB_CAPACITY;
	sb->buffer = (char *)ft_calloc(sb->capacity, sizeof(char));
	if (!sb->buffer)
	{
		sb->capacity = 0;
	}
}

bool	ft_sb_grow(t_string_builder *sb, size_t needed)
{
	char	*new_buffer;
	size_t	new_capacity;

	if (!sb->buffer)
		return (false);
	if (sb->length + needed < sb->capacity)
		return (true);
	new_capacity = sb->capacity;
	while (sb->length + needed >= new_capacity)
		new_capacity *= 2;
	new_buffer = (char *)ft_calloc(new_capacity, sizeof(char));
	if (!new_buffer)
	{
		free(sb->buffer);
		sb->buffer = NULL;
		sb->capacity = 0;
		sb->length = 0;
		return (false);
	}
	ft_memcpy(new_buffer, sb->buffer, sb->length);
	free(sb->buffer);
	sb->buffer = new_buffer;
	sb->capacity = new_capacity;
	return (true);
}

void	ft_sb_append_char(t_string_builder *sb, char c)
{
	if (!ft_sb_grow(sb, 1))
		return ;
	sb->buffer[sb->length] = c;
	sb->length++;
	sb->buffer[sb->length] = '\0';
}

void	ft_sb_append_str(t_string_builder *sb, const char *str)
{
	size_t	str_len;

	if (!str || !sb->buffer)
		return ;
	str_len = ft_strlen(str);
	if (!ft_sb_grow(sb, str_len))
		return ;
	ft_memcpy(sb->buffer + sb->length, str, str_len);
	sb->length += str_len;
	sb->buffer[sb->length] = '\0';
}

char	*ft_sb_build(t_string_builder *sb)
{
	char	*final_str;

	if (!sb->buffer)
		return (NULL);
	final_str = ft_strdup(sb->buffer);
	free(sb->buffer);
	sb->buffer = NULL;
	sb->length = 0;
	sb->capacity = 0;
	return (final_str);
}
