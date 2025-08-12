/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/*   Updated: 2025/08/06 19:44:54 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/lexer.h"

// crea un nuevo nodo de token
static t_token	*create_new_token(char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = (t_token *)ft_calloc(1, sizeof(t_token));
	if (!new_token)
	{
		perror("minishell: malloc failed in create_new_token");
		return(NULL);
	}
	new_token->value = ft_strdup(value);
	if (!new_token->value)
	{
		perror("minishell: ft_strdup failed in create_new_token");
		free(new_token);
		return (NULL);
	}
	new_token->type = type;
	new_token->next = NULL;
	new_token->prev = NULL;
	return (new_token);
}

// Añade un token a la lista enlazada
static void	add_token_to_list(t_token **head, t_token **current,
		t_token *new_token)
{
	if (!new_token)
		return ;
	if (!*head)
	{
		*head = new_token;
		*current = new_token;
	}
	else
	{
		(*current)->next = new_token;
		new_token->prev = *current;
		*current = new_token;
	}
}

void	free_tokens(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		if (tmp->value)
			// Asegurarnos de que value no sea NULL antes de liberar.
			free(tmp->value);
		free(tmp);
	}
}

// Funciones auxiliares para funcion Principal
static int	skip_spaces(const char *input, int i)
{
	while (input[i] && (input[i] == ' ' || input[i] == '\t'))
		i++;
	return (i);
}

static int	handle_redirection_in(const char *input, int i, t_token **head,
		t_token **current)
{
	t_token *new_token;

	if (input[i + 1] == '<')
	{
		new_token = create_new_token("<<", HEREDOC);
		if (!new_token)
			return (-1);
		add_token_to_list(head, current, new_token);
		return (i + 2);
	}
	new_token = create_new_token("<", IN);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}

static int	handle_redirection_out(const char *input, int i, t_token **head,
		t_token **current)
{
	t_token *new_token;

	if (input[i + 1] == '>')
	{
		new_token = create_new_token(">>", APPE_OUT);
		if (!new_token)
			return (-1);
		add_token_to_list(head, current, new_token);
		return (i + 2);
	}
	new_token = create_new_token(">", OUT);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}

static int	handle_pipe(const char *input, int i, t_token **head,
		t_token **current)
{
	t_token *new_token;

	new_token = create_new_token("|", PIPE);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}

static int	skip_quotes(const char *input, int i, char quote_char)
{
	i++;
	while (input[i] && input[i] != quote_char)
		i++;
	if (input[i] == quote_char)
		i++;
	else
		return (-1); // Error: comilla sin cerrar
	return (i);
}

static int	extract_word(const char *input, int i, t_token **head,
		t_struct *mini)
{
	int	new_i;

	while (input[i] && input[i] != ' ' && input[i] != '\t' && input[i] != '<'
		&& input[i] != '>' && input[i] != '|')
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			new_i = skip_quotes(input, i, input[i]);
			if (new_i == -1)
			{
				fprintf(stderr, "minishell: Syntax error: unclosed quote\n");
				mini->last_exit_status = 258;
				return (-1);
			}
			i = new_i;
		}
		else
			i++;
	}
	return (i);
}

static int	handle_word(const char *input, int i, t_token **head,
		t_token **current, t_struct *mini)
{
	int		start;
	int		token_len;
	char	*word_value;
	t_token *new_token;

	start = i;
	i = extract_word(input, i, head, mini);
	if (i < 0)
		return (-1);
	token_len = i - start;
	word_value = (char *)malloc(token_len + 1);
	if (!word_value)
	{
		perror("minishell: malloc failed for word_value");
		free_tokens(*head);
		mini->last_exit_status = 1;
		return (-1);
	}
	ft_strncpy(word_value, (char *)&input[start], token_len);
	word_value[token_len] = '\0';
	new_token = create_new_token(word_value, WORD);
	free(word_value);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i);
}

t_token	*lexer(const char *input, t_struct *mini)
{
	t_token *head = NULL;
	t_token *current = NULL;
	int i = 0;

	if (!input)
		return (NULL);
	while (input[i])
	{
		i = skip_spaces(input, i);
		if (!input[i])
			break ;
		if (input[i] == '&' || input[i] == ';' || input[i] == '(' || input[i] == ')')
		{
    	fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", input[i]);
    	mini->last_exit_status = 258;
    	free_tokens(head);
    	return (NULL);
		}
		if (input[i] == '<')
			i = handle_redirection_in(input, i, &head, &current);
		else if (input[i] == '>')
			i = handle_redirection_out(input, i, &head, &current);
		else if (input[i] == '|')
			i = handle_pipe(input, i, &head, &current);
		else
			i = handle_word(input, i, &head, &current, mini);
		if (i < 0)
		{
			free_tokens(head);
			if (mini->last_exit_status == 0)
				mini->last_exit_status = 1;
			return (NULL);
		}
	}
	return (head);
}