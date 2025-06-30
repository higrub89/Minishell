#ifndef TOKEN_H
# define TOKEN_H

# include <stddef.h>

//Estructura Enum para los tipos de tokens.
typedef enum s_token_type
{
  WORD,       //Una palabra normal.
  PIPE,       //operador |
  REDIR_IN,   //operador <
  REDIR_OUT,  //operador >
  APPEND_OUT, //operador >>
  HEREDOC,    //operador <<

} t_token_type;

//Estructura para un token individual.
typedef struct s_token
{
  char  *value;
  t_token_type type;
  struct s_token *next;
} t_token;


#endif