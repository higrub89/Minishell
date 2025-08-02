#include "../inc/expander.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;
// Inicializa el string builder
static void ft_sb_init(t_string_builder *sb)
{
  sb->length = 0;
  sb->capacity = INITIAL_SB_CAPACITY;
  sb->buffer = (char *)ft_calloc(sb->capacity, sizeof(char));
  if (!sb->buffer)
  {
    sb->capacity = 0;
  }
}

// Asegura que hay suficiente espacio en el buffer.
static bool ft_sb_grow(t_string_builder *sb, size_t needed)
{
  char *new_buffer;
  size_t new_capacity;

  if (!sb->buffer)
    return (false);
  if (sb->length + needed < sb->capacity)
    return (true);
  new_capacity = sb->capacity;
  while (sb->length + needed >= new_capacity)
    new_capacity *= 2;
  new_buffer = (char *)ft_calloc(new_capacity, sizeof(char));
  if (!new_buffer)
  {
    free(sb->buffer);
    sb->buffer = NULL;
    sb->capacity = 0;
    sb->length = 0;
    return (false);
  }
  ft_memcpy(new_buffer, sb->buffer, sb->length);
  free(sb->buffer);
  sb->buffer = new_buffer;
  sb->capacity = new_capacity;
  return (true);
}

// Añade un caracter al string builder
static void ft_sb_append_char(t_string_builder *sb, char c)
{
  if (!ft_sb_grow(sb, 1))
    return ;
  sb->buffer[sb->length] = c;
  sb->length++;
  sb->buffer[sb->length] = '\0';
}

// Añade una cadena al string.
static void ft_sb_append_str(t_string_builder *sb, const char *str)
{
  if (!str || !sb->buffer)
    return ;
  size_t str_len = ft_strlen(str);
  if (!ft_sb_grow(sb, str_len))
    return ;
  ft_memcpy(sb->buffer + sb->length, str, str_len);
  sb->length += str_len;
  sb->buffer[sb->length] = '\0';
}

// Finaliza y retorna la cadena construida, liberando los recursos del builder.
static char*ft_sb_build(t_string_builder *sb)
{
  char *final_str;
  if (!sb->buffer)
    return (NULL);
  final_str = ft_strdup(sb->buffer);
  free(sb->buffer);
  sb->buffer = NULL;
  sb->length = 0;
  sb->capacity = 0;
  return (final_str);
}



static char *get_env_value(const char *name, char **envp)
{
  size_t name_len = ft_strlen(name);
  if (name_len == 0)
    return (NULL);
  while(*envp)
  {
    if (ft_strncmp(*envp, name, name_len) == 0 && (*envp)[name_len] == '=')
      return (*envp + name_len + 1);
    envp++;
  }
  return (NULL);
}

// Fúncion principal.
char  *expand_single_string(char *original_str, char **envp, int last_exit_status)
{
  int i;
  if (!original_str)
      return (ft_strdup(""));

  t_string_builder sb;
  ft_sb_init(&sb);
  if (!sb.buffer) // Error en la inicializacion del buffer.
    return (NULL);
  
  i = 0;
  while (original_str[i]) // Recorre la cadena caracter a caracter
  {  
    if (original_str[i] == '$')
    {
      i++;
      if (original_str[i] == '?') // Expansión $?
      {
        char *exit_str = ft_itoa(last_exit_status);
        if (!exit_str)
        {
          ft_sb_build(&sb);
          return (NULL);
        }
        ft_sb_append_str(&sb, exit_str);
        free(exit_str);
        i++;
      }
      else if (ft_isdigit(original_str[i]))
      {
        ft_sb_append_char(&sb, '$');
        ft_sb_append_char(&sb, original_str[i]);
        i++;
      }
      else if(ft_isalpha(original_str[i]) || original_str[i] == '_') // Expansión de averiables alfanumericos
      {
        int start = i;
        while (ft_isalnum(original_str[i]) || original_str[i] == '_')
          i++;
        char *var_name = ft_substr(original_str, start, i - start);
        if (!var_name)
        {
          ft_sb_build(&sb);
          return (NULL);
        }
        char *var_value = get_env_value(var_name, envp);
        if (var_value)
          ft_sb_append_str(&sb, var_value);
        free(var_name);
      }
      else if (original_str[i] == '\0') // '$' al final de la cadena
      {
        ft_sb_append_char(&sb, '$');
      }
      else
      {
        ft_sb_append_char(&sb, '$');
        ft_sb_append_char(&sb, original_str[i]);
        i++;
      }
    }
    else // caracter normal, no es '$'
    {
      ft_sb_append_char(&sb, original_str[i]);
      i++;
    }
  }
  return (ft_sb_build(&sb));
}
      


// Esta es la funcion que recorre la lista de comandos y sus argu..
void expand_variables(t_command *commands, char **envp, int *last_exit_status_ptr)
{
  t_command *current_cmd;
  t_redirection *current_redir;
  char  *expanded_str;
  int   i;

  current_cmd = commands;
  while (current_cmd) // Recorremos cada comando de la lista.
  {
    if (current_cmd->args)
    {
      i = 0;
      while (current_cmd->args[i])
      {
        expanded_str = expand_single_string(current_cmd->args[i], envp, *last_exit_status_ptr);
        if (!expanded_str)
        {
          *last_exit_status_ptr = 1;
          return ;
        }
        free(current_cmd->args[i]);
        current_cmd->args[i] = expanded_str;
        i++;
      }
    }
    current_redir = current_cmd->redirs;
    while (current_redir) // Recorremos cada argumento
    {
      if (current_redir->type != REDIR_HEREDOC)
      {
        expanded_str = expand_single_string(current_redir->file, envp, *last_exit_status_ptr);
        if (!expanded_str)
        {
          *last_exit_status_ptr = 1;
          return ;       
        }
        free(current_redir->file);
        current_redir->file = expanded_str;
      }
      current_redir = current_redir->next;
    }
    current_cmd = current_cmd->next; //Pasamos al siguiente comando.
  }
}