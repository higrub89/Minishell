/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_executor.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:37:14 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/04 03:15:40 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"
#include "../inc/expander.h"
#include <sys/wait.h> // For waitpid, WIF* macros
#include <unistd.h>   // For fork, pipe, close

void	restore_fds(int stdin_fd, int stdout_fd)
{
	dup2(stdin_fd, STDIN_FILENO);
	close(stdin_fd);
	dup2(stdout_fd, STDOUT_FILENO);
	close(stdout_fd);
}
