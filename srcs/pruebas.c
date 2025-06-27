#include "../inc/minishell.h"

int main(int argc, char **argv, char **env)
{
   (void)argc;
   (void)argv;
   (void)env;
   char *line;
   while(1)
   {
      line = readline("minishell$ ");

      if (line == NULL)
      {
         printf("exit\n");
         break;
      }
      if (strlen(line) > 0)
      {
         add_history(line);
         t_token *tokens = lexer(line);

         if (tokens)
         {
            t_token *current_token = tokens;
            printf("Tokens generados\n");
            while(current_token)
            {
               printf("Value: '%s', You type: '%d'\n", current_token->value, current_token->type);
               current_token = current_token->next;
            }
            free_tokens(tokens);
         }
      }
      free(line);
   }
   return (0);
}
