#include "../inc/expander.h"
#include "../libft/inc/libft.h"

static char *ft_strjoin_and_free_first(char *s1, char *s2)
{
  char *new_str;

  if (!s1)
    return (ft_strdup(s2));
  if (!s2)
    return (s1);

  new_str = ft_strjoin(s1, s2);
  free(s1);
  return (new_str);
}

static int get_var_name_len(const char *s)
{
  int len;

  len = 0;
  if (s[0] == '?')
    return (1);
  if (!ft_isalpha(s[0]) && s[0] != '_')
    return (0); // No es un nombre de variable válido.
  while (ft_isalnum(s[len]) || s[len] == '_')
    len++;
  return (len);
}

char  *expand_string(char *original_str, char **envp, int last_exit_status)
{
  char *expanded_str;
  char *var_name;
  char *var_value;
  char *var_value_raw;
  char *var_value_dup;
  int i;
  int start_exp;
  int len_var_name;
  char in_single_quotes;
  char in_double_quotes;

  if (!original_str)
    return (NULL);
  
  expanded_str = ft_strdup(""); // Inicializa con una cadena vacia;
  if (!expanded_str)
    return (NULL);

  i = 0;
  in_single_quotes = 0;
  in_double_quotes = 0;
  while (original_str[i])
  {
    if (original_str[i] == '\'')
    {
      if (!in_double_quotes)
        in_single_quotes = !in_single_quotes;
      i++;
      continue;
    }
    else if (original_str[i] == '"') 
    {
      if (!in_single_quotes)
        in_double_quotes = !in_double_quotes;
      i++;
      continue;
    }
    else if (original_str[i] == '$' && !in_single_quotes)
    { 
      start_exp = i + 1;
      len_var_name = get_var_name_len(original_str + start_exp);
      if (len_var_name == 0)
      {
        // manejar $?
        if (original_str[start_exp] == '?')
        {
          var_value = ft_itoa(last_exit_status);
          if (!var_value)
          {
            free(expanded_str);
            return (NULL);
          }
          expanded_str = ft_strjoin_and_free_first(expanded_str, var_value);
          free(var_value);
          if (!expanded_str)
            return (NULL);
          i += 2;
        }
        else if (original_str[start_exp] == '\0' ||
                ft_isspace(original_str[start_exp]))
        {
          expanded_str = ft_strjoin_and_free_first(expanded_str, ft_strdup("$"));
          if (!expanded_str)
            return (NULL);
          i++;
        }
        else
        {
          char *dollar_char_literal = ft_substr(original_str, i, 2);
          if (!dollar_char_literal)
          {
            free(expanded_str);
            return (NULL);
          }
          expanded_str = ft_strjoin_and_free_first(expanded_str, dollar_char_literal);
          free(dollar_char_literal);
          if (!expanded_str)
            return (NULL);
          i += 2; // saltar '$' y el caracter
        }
      }
      else // Nombre de variable válido.
      {
        var_name = ft_substr(original_str, start_exp, len_var_name);
        if (!var_name)
        {
          free(expanded_str);
          return (NULL);
        }
        var_value_raw = getenv(var_name); // obtener el valor de la variable.
        free(var_name);
        if (var_value_raw) // si la variable existe concatenamos su valor.
        {
          var_value_dup = ft_strdup(var_value_raw);
          if (!var_value_dup)
          {
            free(expanded_str);
            return (NULL);
          }
          expanded_str = ft_strjoin_and_free_first(expanded_str, var_value_dup);
          free(var_value_dup);
          if (!expanded_str)
            return (NULL);
        }
        i += (1 + len_var_name);
      }
    }
    else
    {
      char *char_str = ft_substr(original_str, i, 1);
      if (!char_str)
      {
        free(expanded_str);
        return (NULL);
      }
      expanded_str = ft_strjoin_and_free_first(expanded_str, char_str);
      free(char_str);
      if (!expanded_str)
        return (NULL);
      i++;
    }
  }
  return (expanded_str);
}