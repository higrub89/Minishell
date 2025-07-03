#include "../inc/minishell.h"

int main(void)
{
   char *input_line;
   t_token *tokens;
   t_command *commands;

   while(1)
   {
      input_line = readline("minishell> ");
      if (!input_line)
      {
         printf("exit\n");
         break;
      }
      if (*input_line)
         add_history(input_line);
      tokens = lexer(input_line);
      if (!tokens)
      {
         free(input_line);
         continue;
      }
      commands = parse_input(tokens);
      if (!commands)
      {
         free_tokens(tokens);
         free(input_line);
         continue;
      }
         // Executor....
      free_commands(commands);
      free_tokens(tokens);
      free(input_line);
   }
   return (0);
}
