#include "../inc/builtins.h"

/**
 * @brief Verifica si una ruta es un directorio válido y accesible.
 * Utiliza stat() para obtener información sobre la ruta y comprueba si es un directorio.
 * @param path La ruta a verificar.
 * @return 1 si es un directorio válido y accesible, 0 en caso contrario.
 */
static int is_valid_directory(char *path)
{
    struct stat info;

    // stat() intenta obtener información del archivo/directorio.
    // Retorna 0 si es exitoso, -1 si hay un error (ej. no existe, permisos).
    if (stat(path, &info) == 0)
    {
        // S_ISDIR() es una macro que comprueba si el modo del archivo indica que es un directorio.
        if (S_ISDIR(info.st_mode))
            return (1); // Es un directorio válido
    }
    return (0); // No existe o no es un directorio
}

/**
 * @brief Actualiza las variables de entorno PWD y OLDPWD después de un cambio de directorio.
 * Utiliza ft_setenv_var (de env_utils) para establecer los nuevos valores.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param new_path La nueva ruta canónica del directorio actual.
 */
static void update_pwd_env(t_struct *mini, const char *new_path)
{
    char *old_pwd_value;

    // 1. Obtener el valor actual de PWD para convertirlo en OLDPWD
    // Usamos get_env_value de env_utils.h
    old_pwd_value = get_env_value("PWD", mini);

    // 2. Establecer OLDPWD con el valor anterior de PWD
    // Si PWD existía, su valor actual se convierte en el nuevo OLDPWD.
    // Si PWD no existía, OLDPWD se establece como una cadena vacía.
    if (old_pwd_value)
        ft_setenv_var(mini, "OLDPWD", old_pwd_value);
    else
        ft_setenv_var(mini, "OLDPWD", ""); // Si no hay PWD previo, OLDPWD se deja vacío

    // 3. Establecer PWD con la nueva ruta canónica
    ft_setenv_var(mini, "PWD", new_path);
}

/**
 * @brief Cambia el directorio actual al directorio HOME del usuario.
 * Busca la variable de entorno HOME. Si no está configurada, imprime un error.
 * @param mini Puntero a la estructura principal de la minishell.
 * @return 0 en éxito, 1 en error.
 */
static int go_home(t_struct *mini)
{
    char *home_path;

    // Obtiene el valor de la variable HOME usando get_env_value de env_utils.h
    home_path = get_env_value("HOME", mini);
    if (!home_path || home_path[0] == '\0') // También considera HOME=""
    {
        ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
        mini->last_exit_status = 1;
        return (1);
    }
    
    // Intenta cambiar el directorio
    if (chdir(home_path) == 0)
    {
        char *canonical_path = getcwd(NULL, 0);
        if (canonical_path) {
            update_pwd_env(mini, canonical_path); // Actualiza PWD y OLDPWD
            free(canonical_path);
            mini->last_exit_status = 0;
            return (0);
        } else {
            perror("minishell: cd: getcwd failed");
            mini->last_exit_status = 1;
            return (1);
        }
    }
    else
    {
        // Si chdir falla (ej. HOME no existe, permisos insuficientes), imprime error.
        perror("minishell: cd"); // perror añade el mensaje de error del sistema
        mini->last_exit_status = 1;
        return (1);
    }
}

/**
 * @brief Obtiene la ruta del directorio padre a partir del PWD actual.
 * Maneja el caso de la raíz ('/') correctamente.
 * @param mini Puntero a la estructura principal de la minishell.
 * @return Una string con la ruta del padre (duplicada), o NULL si hay un error.
 */
static char *get_parent_path(t_struct *mini)
{
    char *pwd;
    char *parent;
    int len;

    // Obtiene el valor de PWD usando get_env_value de env_utils.h
    pwd = get_env_value("PWD", mini);
    
    // Si PWD no está configurado en el entorno, intenta obtener la ruta con getcwd().
    // Esto es un fallback, aunque normalmente PWD debería estar siempre actualizado por cd.
    if (!pwd)
    {
        pwd = getcwd(NULL, 0); // Permite a getcwd asignar memoria
        if (!pwd) {
            perror("minishell: cd: getcwd failed"); // Error grave si falla getcwd
            return (NULL);
        }
    }
    else
    {
        // Si pwd existe, lo duplicamos para poder modificarlo o usarlo con ft_substr
        // y que no apunte directamente al envp de mini.
        pwd = ft_strdup(pwd);
        if (!pwd) {
            perror("minishell: cd: strdup failed");
            return (NULL);
        }
    }

    len = ft_strlen(pwd);
    // Retrocede desde el final hasta encontrar la última barra (excepto si es la raíz)
    while (len > 0 && pwd[len - 1] != '/')
        len--;
    
    // Si la ruta es "/" o solo tiene un componente (ej. "/bin"), el padre es "/".
    // Si len es 0 (ej. "foo" sin slash inicial), o len es 1 (ej. "/"), el padre es "/".
    if (len <= 1 && pwd[0] == '/')
        parent = ft_strdup("/");
    else if (len == 0 && pwd[0] != '/') // Caso de una ruta relativa sin "/" inicial, ej. "dir"
        parent = ft_strdup("."); // El padre es el directorio actual
    else
        parent = ft_substr(pwd, 0, len - 1); // Extrae la subcadena hasta la última '/'

    free(pwd); // Libera la copia de pwd o la memoria de getcwd.
    return (parent);
}

/**
 * @brief Cambia el directorio actual al directorio padre ('..').
 * @param mini Puntero a la estructura principal de la minishell.
 * @return 0 en éxito, 1 en error.
 */
static int go_to_parent(t_struct *mini)
{
    char *new_dir;

    new_dir = get_parent_path(mini);
    if (!new_dir)
    {
        mini->last_exit_status = 1;
        return (1); // Error ya reportado por get_parent_path
    }
    
    // Intenta cambiar el directorio al padre
    if (chdir(new_dir) == 0)
    {
        char *canonical_path = getcwd(NULL, 0); // Obtener la ruta canónica después del chdir
        if (canonical_path) {
            update_pwd_env(mini, canonical_path); // Actualiza PWD y OLDPWD con la ruta canónica
            free(canonical_path);
            mini->last_exit_status = 0;
        } else {
            // Si getcwd falla después de un chdir exitoso, es un problema serio
            perror("minishell: cd: getcwd failed after chdir");
            mini->last_exit_status = 1;
        }
    }
    else
    {
        perror("minishell: cd"); // Error si el directorio padre no es accesible
        mini->last_exit_status = 1;
    }
    free(new_dir); // Libera la string de la ruta del padre
    return (mini->last_exit_status);
}

/**
 * @brief Cambia el directorio actual a la ruta almacenada en OLDPWD ('cd -').
 * Imprime la ruta a la que se ha cambiado, como lo hace Bash.
 * @param mini Puntero a la estructura principal de la minishell.
 * @return 0 en éxito, 1 en error.
 */
static int go_to_oldpwd(t_struct *mini)
{
    char *oldpwd_path;

    // Obtiene el valor de OLDPWD usando get_env_value de env_utils.h
    oldpwd_path = get_env_value("OLDPWD", mini);
    if (!oldpwd_path || oldpwd_path[0] == '\0') // También considera OLDPWD=""
    {
        ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
        mini->last_exit_status = 1;
        return (1);
    }

    // Intenta cambiar el directorio al valor de OLDPWD
    if (chdir(oldpwd_path) == 0)
    {
        ft_putendl_fd(oldpwd_path, STDOUT_FILENO); // Bash imprime la ruta al usar 'cd -'
        char *canonical_path = getcwd(NULL, 0); // Obtener la ruta canónica después del chdir
        if (canonical_path) {
            update_pwd_env(mini, canonical_path); // Actualiza PWD y OLDPWD
            free(canonical_path);
            mini->last_exit_status = 0;
        } else {
            perror("minishell: cd: getcwd failed after chdir");
            mini->last_exit_status = 1;
        }
        return (mini->last_exit_status);
    }
    else
    {
        // Si chdir falla, imprime error
        perror("minishell: cd");
        mini->last_exit_status = 1;
        return (1);
    }
}

/**
 * @brief Implementación del builtin 'cd'.
 * Cambia el directorio de trabajo actual según los argumentos.
 * Gestiona 'cd' (a HOME), 'cd -' (a OLDPWD), 'cd ..', y rutas absolutas/relativas.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos (args[0] es "cd", args[1] es la ruta opcional).
 * @return 0 en éxito, 1 en error.
 */
int ft_cd(t_struct *mini, char **args)
{
    char *target_path;

    target_path = args[1]; // El argumento de la ruta es args[1]

    // Caso 1: 'cd' sin argumentos o 'cd ""' -> Ir a HOME
    if (!target_path || target_path[0] == '\0')
        return (go_home(mini));

    // Caso 2: 'cd -' -> Ir a OLDPWD
    if (ft_strncmp(target_path, "-", 2) == 0) // Uso 2 para comparar también el NULL terminator
        return (go_to_oldpwd(mini));
    
    // Caso 3: 'cd ..' -> Ir al directorio padre
    if (ft_strncmp(target_path, "..", 3) == 0) // Uso 3 para comparar también el NULL terminator
        return (go_to_parent(mini));

    // Caso 4: 'cd .' o 'cd ./' -> No hacer nada, pero actualizar PWD si es necesario
    if (ft_strncmp(target_path, ".", 2) == 0 || ft_strncmp(target_path, "./", 3) == 0)
    {
        char *current_cwd = getcwd(NULL, 0);
        if (current_cwd) {
            update_pwd_env(mini, current_cwd); // Asegura que PWD esté actualizado
            free(current_cwd);
            mini->last_exit_status = 0;
            return (0);
        } else {
            perror("minishell: cd: getcwd failed");
            mini->last_exit_status = 1;
            return (1);
        }
    }
    
    // Caso 5: Ruta absoluta o relativa (ej. /tmp, mydir, ../another)
    // Se añade esta validación para usar la función y evitar el error del compilador.
    // Además, mejora el manejo de errores.
    if (is_valid_directory(target_path) == 0)
    {
        perror("minishell: cd");
        mini->last_exit_status = 1;
        return (1);
    }

    // Se intenta el cambio de directorio directamente con chdir.
    if (chdir(target_path) == 0)
    {
        // Si chdir es exitoso, obtenemos la ruta canónica para actualizar PWD.
        char *canonical_path = getcwd(NULL, 0); 
        if (canonical_path) {
            update_pwd_env(mini, canonical_path); // Actualiza PWD y OLDPWD
            free(canonical_path);
            mini->last_exit_status = 0;
            return (0);
        } else {
            // Si getcwd falla después de un chdir exitoso, es un problema grave.
            perror("minishell: cd: getcwd failed after chdir");
            mini->last_exit_status = 1;
            return (1);
        }
    }
    else // chdir falló
    {
        // Esto solo debería ocurrir por problemas de permisos si la validación anterior pasó.
        perror("minishell: cd"); 
        mini->last_exit_status = 1;
        return (1);
    }
}