#include "../inc/expander.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;

static char *get_env_value(const char *name, char **envp)
{
  size_t name_len = ft_strlen(name);
  while(*envp)
  {
    if (ft_strncmp(*envp, name, name_len) == 0 && (*envp)[name_len] == '=')
      return (*envp + name_len);
  }
  return (NULL);
}

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
  (void)envp;
    char *expanded_str;
    char *var_name;
    char *var_value_raw; // Puntero al valor directamente de getenv (no se libera)
    char *var_value_dup; // Copia del valor de la variable (para liberar)
    int i;
    int start_literal; // Índice de inicio de la parte literal actual
    char in_single_quotes;
    char in_double_quotes;

    // Si la cadena de entrada es NULL, no hay nada que expandir.
    if (!original_str)
        return (NULL);
    
    // Inicializa la cadena expandida como una cadena vacía.
    expanded_str = ft_strdup("");
    if (!expanded_str)
        return (NULL); // Fallo de malloc inicial

    i = 0;
    start_literal = 0; // La primera parte literal comienza al inicio
    in_single_quotes = 0;
    in_double_quotes = 0;

    while (original_str[i]) // Recorre la cadena caracter a caracter
    {
        // 1. Manejo de Comillas Simples (ignora si estamos en comillas dobles)
        if (original_str[i] == '\'' && !in_double_quotes)
        {
            // Añade la parte literal acumulada antes de la comilla
            if (i > start_literal)
            {
                char *literal_part = ft_substr(original_str, start_literal, i - start_literal);
                if (!literal_part) { free(expanded_str); return (NULL); }
                expanded_str = ft_strjoin_and_free_first(expanded_str, literal_part);
                free(literal_part); // ft_strjoin_and_free_first ya liberó el viejo expanded_str
                if (!expanded_str) return (NULL);
            }
            in_single_quotes = !in_single_quotes; // Alterna el estado
            i++; // Saltar la comilla simple
            start_literal = i; // Reiniciar el inicio literal después de la comilla
            continue; // Ir al siguiente caracter
        }
        // 2. Manejo de Comillas Dobles (ignora si estamos en comillas simples)
        else if (original_str[i] == '"' && !in_single_quotes)
        {
            // Añade la parte literal acumulada antes de la comilla
            if (i > start_literal)
            {
                char *literal_part = ft_substr(original_str, start_literal, i - start_literal);
                if (!literal_part) { free(expanded_str); return (NULL); }
                expanded_str = ft_strjoin_and_free_first(expanded_str, literal_part);
                free(literal_part);
                if (!expanded_str) return (NULL);
            }
            in_double_quotes = !in_double_quotes; // Alterna el estado
            i++; // Saltar la comilla doble
            start_literal = i; // Reiniciar el inicio literal después de la comilla
            continue; // Ir al siguiente caracter
        }
        // 3. Manejo de '$' (solo si NO estamos dentro de comillas simples)
        else if (original_str[i] == '$' && !in_single_quotes)
        { 
            // Añade la parte literal acumulada antes del '$'
            if (i > start_literal)
            {
                char *literal_part = ft_substr(original_str, start_literal, i - start_literal);
                if (!literal_part) { free(expanded_str); return (NULL); }
                expanded_str = ft_strjoin_and_free_first(expanded_str, literal_part);
                free(literal_part);
                if (!expanded_str) return (NULL);
            }

            int start_exp = i + 1; // El caracter justo después del '$'
            int len_var_name = get_var_name_len(original_str + start_exp); // Longitud del nombre de la variable

            if (len_var_name == 0) // No es un nombre de variable válido (ej. $!, $1, $", o solo $)
            {
                if (original_str[start_exp] == '?') // Caso especial: $?
                {
                    var_value_dup = ft_itoa(last_exit_status);
                    if (!var_value_dup) { free(expanded_str); return (NULL); }
                    expanded_str = ft_strjoin_and_free_first(expanded_str, var_value_dup);
                    free(var_value_dup); // Liberar la cadena de ft_itoa
                    if (!expanded_str) return (NULL);
                    i += 2; // Saltar '$' y '?'
                }
                // Si '$' está al final de la cadena o seguido de espacio, se trata como un '$' literal.
                else if (original_str[start_exp] == '\0' || ft_isspace(original_str[start_exp]))
                {
                    expanded_str = ft_strjoin_and_free_first(expanded_str, ft_strdup("$"));
                    if (!expanded_str) return (NULL);
                    i++; // Saltar solo '$'
                }
                // Si '$' está seguido de cualquier otro caracter que no forma un nombre de variable válido (ej. $!, $#)
                // y no es un espacio o fin de cadena, se trata como '$' seguido de ese caracter literal.
                else
                {
                    char *dollar_char_literal = ft_substr(original_str, i, 2); // Copiar "$X"
                    if (!dollar_char_literal) { free(expanded_str); return (NULL); }
                    expanded_str = ft_strjoin_and_free_first(expanded_str, dollar_char_literal);
                    free(dollar_char_literal);
                    if (!expanded_str) return (NULL);
                    i += 2; // Saltar '$' y el caracter
                }
            }
            else // Es un nombre de variable válido (ej. $USER, $PATH)
            {
                var_name = ft_substr(original_str, start_exp, len_var_name);
                if (!var_name) { free(expanded_str); return (NULL); } // Fallo de malloc

                var_value_raw = getenv(var_name); // Obtener el valor de la variable
                free(var_name); // Liberar el nombre de la variable que hemos extraído

                if (var_value_raw) // Si la variable existe en el entorno
                {
                    var_value_dup = ft_strdup(var_value_raw); // Duplicar el valor para poder liberarlo
                    if (!var_value_dup) { free(expanded_str); return (NULL); } // Fallo de malloc
                    expanded_str = ft_strjoin_and_free_first(expanded_str, var_value_dup);
                    free(var_value_dup); // Liberar la copia duplicada
                    if (!expanded_str) return (NULL);
                }
                // Si la variable no existe (var_value_raw es NULL), no se añade nada (se expande a vacío),
                // que es el comportamiento esperado en Bash.
                
                i += (1 + len_var_name); // Avanzar el índice: '$' + longitud del nombre de la variable
            }
            start_literal = i; // Reiniciar el inicio literal después de la expansión
        }
        else // Caracteres normales (no son comillas ni '$', o '$' dentro de comillas simples)
        {
            i++; // Simplemente avanzar. Estos caracteres se copiarán como parte del siguiente literal.
        }
    }

    // Al finalizar el bucle, añade cualquier parte literal restante al final de la cadena expandida.
    if (i > start_literal)
    {
        char *literal_part = ft_substr(original_str, start_literal, i - start_literal);
        if (!literal_part) { free(expanded_str); return (NULL); }
        expanded_str = ft_strjoin_and_free_first(expanded_str, literal_part);
        free(literal_part);
        if (!expanded_str) return (NULL);
    }
    return (expanded_str); // Retorna la cadena final expandida
}

// Esta es la funcion que recorre la lista de comandos y sus argu..
void expand_variables(t_command *commands, char **envp, int last_exit_status)
{
  t_command *currend_cmd;
  char  **current_arg_array;
  char  *expanded_arg;
  int   i;

  currend_cmd = commands;
  while (currend_cmd) // Recorremos cada comando de la lista.
  {
    current_arg_array = currend_cmd->args;
    i = 0;
    while (current_arg_array && current_arg_array[i]) // Recorremos cada argumento
    {
      if (ft_strchr(current_arg_array[i], '$'))
      {
        expanded_arg = expand_string(current_arg_array[i], envp, last_exit_status);
        if (!expanded_arg)
        {
          perror("minishel: Fatal error during Variable expansión");
          return ;       
        }
        free(current_arg_array[i]);
        current_arg_array[i] = expanded_arg;
      }
      i++;
    }
    currend_cmd = currend_cmd->next; //Pasamos al siguiente comando.
  }
}