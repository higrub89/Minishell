#include "../inc/parser.h"
#include "../inc/libft.h"

t_command *parse_input(t_token *token_list)
{
  t_command *head_cmd = NULL;
  t_command *current_cmd = NULL;
  t_token *current_token = token_list;

  if (!token_list)
    return (NULL);
  while (current_token)
  {
    if (!current_cmd)
    {
      
    }
  }
}
