#include "../inc/minishell.h"

int main(int argc, char **argv, char **envp)
{
   char *input_line;
   t_token *tokens;
   t_command *commands;
   int last_exit_status;

   (void)argc;
   (void)argv;

   last_exit_status = 0;

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
         last_exit_status = 2;
         continue;
      }
      commands = parse_input(tokens);
      if (!commands)
      {
         free(input_line);
         last_exit_status = 2;
         continue;
      }
      expand_variables(commands, envp, last_exit_status);
      if (commands && commands->args && commands->args[0])
      {
         last_exit_status = execute_external_command(commands, &last_exit_status);
      }
      else
      {
         last_exit_status = 0;
      }
      free_commands(commands);
      free(input_line);
   }
   return (last_exit_status);
}
