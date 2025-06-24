#include "../inc/minishell.h"

int main(int argc, char **argv, char **env)
{
   (void)argc;
   (void)argv;
   (void)env;
   char *line;
   while(1)
   {
      line = readline("minishell> ");

      if (line == NULL)
      {
         printf("exit\n");
         break;
      }
      if (strlen(line) > 0)
      {
         add_history(line);
         printf("You typed: '%s'\n", line);
      }
      free(line);
   }
   return (0);
}
