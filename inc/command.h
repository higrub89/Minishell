#ifndef COMMAND_H
# define COMMAND_H

# include <stddef.h> // para size_t;

//estructura Enum para los 
typedef enum s_redir_type
{
  REDIR_IN,     // <
  REDIR_OUT,    // >
  REDIR_APPEND, // >>
  REDIR_HEREDOC // <<
} t_redir_type;

// Estructura para una única redirección.
typedef struct s_redirection
{
  t_redir_type type;
  char    *file;
  struct s_redirection *next;
} t_redirection;

// Estructura para un comando completo.
typedef struct s_command
{
  char    **args;
  t_redirection *redirs;
  struct s_command *next;
} t_command;

#endif