/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* expander.c                                         :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/* Updated: 2025/06/24 16:34:10 by rhiguita         ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "../inc/expander.h" // Asegúrate de que esta cabecera incluye t_string_builder y sus definiciones
#include "../libft/inc/libft.h" // Asumo que libft.h tiene ft_strlen, ft_memcpy, ft_strdup, ft_substr, ft_itoa, ft_isdigit, ft_isalpha, ft_isalnum, ft_calloc, free

// Variable global para el estado de salida del último comando
extern int g_last_exit_status; 

// --- Funciones de String Builder ---

// Inicializa el string builder
static void ft_sb_init(t_string_builder *sb)
{
    sb->length = 0;
    sb->capacity = INITIAL_SB_CAPACITY; // Asegúrate de que INITIAL_SB_CAPACITY está definido en expander.h (ej. #define INITIAL_SB_CAPACITY 128)
    sb->buffer = (char *)ft_calloc(sb->capacity, sizeof(char));
    if (!sb->buffer)
    {
        sb->capacity = 0;
        // Podrías setear un flag de error o g_last_exit_status aquí si lo consideras fatal.
    }
}

// Asegura que hay suficiente espacio en el buffer.
static bool ft_sb_grow(t_string_builder *sb, size_t needed)
{
    char *new_buffer;
    size_t new_capacity;

    if (!sb->buffer) // Si el buffer inicializó con error
        return (false);
    if (sb->length + needed < sb->capacity) // Ya hay suficiente espacio
        return (true);
    
    new_capacity = sb->capacity;
    while (sb->length + needed >= new_capacity) // Duplicar capacidad hasta que sea suficiente
        new_capacity *= 2;
    
    new_buffer = (char *)ft_calloc(new_capacity, sizeof(char));
    if (!new_buffer) // Error de malloc
    {
        free(sb->buffer);
        sb->buffer = NULL;
        sb->capacity = 0;
        sb->length = 0;
        return (false);
    }
    ft_memcpy(new_buffer, sb->buffer, sb->length); // Copiar contenido existente
    free(sb->buffer); // Liberar buffer antiguo
    sb->buffer = new_buffer;
    sb->capacity = new_capacity;
    return (true);
}

// Añade un caracter al string builder
static void ft_sb_append_char(t_string_builder *sb, char c)
{
    if (!ft_sb_grow(sb, 1))
        return ; // Manejar error de crecimiento (ej. setear g_last_exit_status)
    sb->buffer[sb->length] = c;
    sb->length++;
    sb->buffer[sb->length] = '\0'; // Mantener null-terminated
}

// Añade una cadena al string builder
static void ft_sb_append_str(t_string_builder *sb, const char *str)
{
    if (!str || !sb->buffer)
        return ; // Manejar error si str es NULL o sb->buffer es NULL
    size_t str_len = ft_strlen(str);
    if (!ft_sb_grow(sb, str_len))
        return ; // Manejar error de crecimiento
    ft_memcpy(sb->buffer + sb->length, str, str_len);
    sb->length += str_len;
    sb->buffer[sb->length] = '\0'; // Mantener null-terminated
}

// Finaliza y retorna la cadena construida, liberando los recursos del builder.
static char* ft_sb_build(t_string_builder *sb)
{
    char *final_str;
    if (!sb->buffer)
        return (NULL); // Si hubo un error en la inicialización o crecimiento
    final_str = ft_strdup(sb->buffer); // Duplicar la cadena final
    free(sb->buffer); // Liberar el buffer interno del builder
    sb->buffer = NULL;
    sb->length = 0;
    sb->capacity = 0;
    return (final_str);
}

// --- Funciones de Expansión ---

// Obtiene el valor de una variable de entorno
static char *get_env_value(const char *name, char **envp)
{
    size_t name_len = ft_strlen(name);
    if (name_len == 0) // Si el nombre de la variable es vacío, no hay valor.
        return (NULL);
    while(*envp)
    {
        // Compara el nombre de la variable y verifica que el siguiente carácter sea '='
        if (ft_strncmp(*envp, name, name_len) == 0 && (*envp)[name_len] == '=')
            return (*envp + name_len + 1); // Retorna el puntero al valor
        envp++;
    }
    return (NULL); // Variable no encontrada
}

// Función principal para expandir una cadena única (dentro o fuera de comillas dobles)
// ESTA FUNCIÓN ES PURA: SOLO EXPANDIRÁ '$' y '?' si los encuentra.
// No se preocupa por las comillas externas; eso lo maneja `expand_variables`.
char *expand_single_string(char *original_str, char **envp, int last_exit_status)
{
    int i;
    
    if (!original_str)
        return (ft_strdup("")); // Retorna una cadena vacía para NULL

    t_string_builder sb;
    ft_sb_init(&sb);
    if (!sb.buffer) // Error en la inicialización del buffer
        return (NULL); 
    
    i = 0;
    while (original_str[i]) // Recorre la cadena carácter a carácter
    {   
        if (original_str[i] == '$')
        {
            i++; // Avanza más allá del '$'
            if (original_str[i] == '?') // Expansión de $? (exit status)
            {
                char *exit_str = ft_itoa(last_exit_status);
                if (!exit_str)
                {
                    ft_sb_build(&sb); // Limpiar builder si falla itoa
                    return (NULL);
                }
                ft_sb_append_str(&sb, exit_str);
                free(exit_str); // Liberar la cadena de itoa
                i++; // Avanza más allá del '?'
            }
            else if (ft_isdigit(original_str[i])) // Expansión de $0, $1, etc. (parámetros posicionales)
            {
                // Según el sujeto, no necesitas implementar $1, $2, etc.
                // Bash los ignora o los usa para parámetros del script.
                // Aquí, simplemente los pasamos literalmente, o podrías ignorarlos.
                ft_sb_append_char(&sb, '$'); // Mantener el '$'
                ft_sb_append_char(&sb, original_str[i]); // Mantener el dígito
                i++;
            }
            else if(ft_isalpha(original_str[i]) || original_str[i] == '_') // Expansión de variables alfanuméricas (ej. $USER, $HOME)
            {
                int start = i; // Guarda el inicio del nombre de la variable
                while (ft_isalnum(original_str[i]) || original_str[i] == '_')
                    i++; // Avanza mientras sea un carácter válido de nombre de variable
                char *var_name = ft_substr(original_str, start, i - start);
                if (!var_name)
                {
                    ft_sb_build(&sb);
                    return (NULL);
                }
                char *var_value = get_env_value(var_name, envp);
                if (var_value) // Si la variable existe, añadir su valor
                    ft_sb_append_str(&sb, var_value);
                // Si var_value es NULL (variable no definida), no se añade nada (se expande a vacío).
                free(var_name); // Liberar el nombre de la variable extraído
            }
            else if (original_str[i] == '\0') // '$' al final de la cadena (ej. "hola$")
            {
                ft_sb_append_char(&sb, '$'); // Mantener el '$' literal
            }
            else // Carácter después de '$' que no es ?, dígito, letra o _
            {
                ft_sb_append_char(&sb, '$'); // Mantener el '$' literal
                ft_sb_append_char(&sb, original_str[i]); // Mantener el carácter literal
                i++;
            }
        }
        else // Carácter normal, no es '$'
        {
            ft_sb_append_char(&sb, original_str[i]);
            i++;
        }
    }
    return (ft_sb_build(&sb)); // Construye y retorna la cadena final
}

// NUEVA FUNCIÓN AUXILIAR para quitar las comillas externas
// y determinar si el contenido de la cadena debe ser expandido.
// Retorna una cadena nueva (strdup o substr) que el llamador debe liberar.
// `do_expand` es una bandera de salida que indica si el contenido debe expandirse.
static char *unquote_and_determine_expansion(char *raw_str, bool *do_expand)
{
    size_t len = ft_strlen(raw_str);

    // Caso: Cadena entre comillas simples (ej. "'$USER'")
    // El lexer debe asegurar que la cadena completa llegue aquí, incluyendo las comillas.
    if (len >= 2 && raw_str[0] == '\'' && raw_str[len - 1] == '\'')
    {
        *do_expand = false; // Contenido **NO** se expande
        return ft_substr(raw_str, 1, len - 2); // Devuelve el contenido sin las comillas
    }
    // Caso: Cadena entre comillas dobles (ej. ""$USER"")
    else if (len >= 2 && raw_str[0] == '"' && raw_str[len - 1] == '"')
    {
        *do_expand = true; // Contenido **SÍ** se expande
        return ft_substr(raw_str, 1, len - 2); // Devuelve el contenido sin las comillas
    }
    // Caso: Sin comillas o comillas mal formadas/mezcladas
    // Aquí, asumimos que el contenido **SÍ** se expande, como en Bash.
    else
    {
        *do_expand = true; 
        return ft_strdup(raw_str); // Devuelve una copia tal cual (se necesita una copia propia para expand_single_string)
    }
}

// Función que recorre la lista de comandos y sus argumentos/redirecciones
// para aplicar la expansión de variables.
void expand_variables(t_command *commands, char **envp, int *last_exit_status_ptr)
{
    t_command       *current_cmd;
    t_redirection   *current_redir;
    char            *processed_str;       // Cadena después de quitar comillas externas
    char            *final_str;           // Cadena final después de la expansión
    bool            should_expand_content; // Bandera para controlar si se llama a expand_single_string
    int             i;

    current_cmd = commands;
    while (current_cmd) // Recorrer cada comando en la pipeline
    {
        // 1. Expandir argumentos del comando
        if (current_cmd->args)
        {
            i = 0;
            while (current_cmd->args[i])
            {
                // Paso A: "Descomillar" el argumento y decidir si su contenido se expande
                processed_str = unquote_and_determine_expansion(current_cmd->args[i], &should_expand_content);
                if (!processed_str) // Manejo de error de malloc
                {
                    *last_exit_status_ptr = 1; // Setear código de error
                    return; // Podrías necesitar un manejo de errores más sofisticado aquí
                }

                // Paso B: Si se debe expandir, llamar a expand_single_string
                if (should_expand_content)
                {
                    final_str = expand_single_string(processed_str, envp, *last_exit_status_ptr);
                    free(processed_str); // Liberar la cadena temporal `processed_str`
                }
                else
                {
                    // Si no se expande, `processed_str` ya es la cadena final.
                    final_str = processed_str; // `final_str` ahora apunta a `processed_str`
                }
                
                if (!final_str) // Manejo de error de malloc en expansión o copia
                {
                    *last_exit_status_ptr = 1;
                    return;
                }
                
                // Reemplazar el argumento original con la cadena final (expandida o literal)
                free(current_cmd->args[i]);
                current_cmd->args[i] = final_str;
                i++;
            }
        }
        
        // 2. Expandir nombres de archivo en redirecciones (excepto heredoc)
        current_redir = current_cmd->redirs;
        while (current_redir)
        {
            // La lógica del delimitador de HEREDOC y su expansión ya se maneja en `parser.c`
            // en `procces_heredoc_input`. Por eso, excluimos `REDIR_HEREDOC` aquí.
            if (current_redir->type != REDIR_HEREDOC)
            {
                // Paso A: "Descomillar" el nombre de archivo y decidir si se expande
                processed_str = unquote_and_determine_expansion(current_redir->file, &should_expand_content);
                if (!processed_str)
                {
                    *last_exit_status_ptr = 1;
                    return;
                }

                // Paso B: Si se debe expandir, llamar a expand_single_string
                if (should_expand_content)
                {
                    final_str = expand_single_string(processed_str, envp, *last_exit_status_ptr);
                    free(processed_str);
                }
                else
                {
                    final_str = processed_str;
                }
                
                if (!final_str)
                {
                    *last_exit_status_ptr = 1;
                    return;
                }
                
                // Reemplazar el nombre de archivo original con la cadena final
                free(current_redir->file);
                current_redir->file = final_str;
            }
            current_redir = current_redir->next;
        }
        current_cmd = current_cmd->next; // Pasar al siguiente comando en la lista
    }
}