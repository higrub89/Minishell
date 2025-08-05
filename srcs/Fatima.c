
FX UNSET

 

int is_valid_var_name(char *argv)

{

    int i = 0;

    if (!argv || ft_isdigit(argv[i]))

        return (0);

    

    while (argv[i])

    {

        if (!ft_isalnum(argv[i]) && argv[i] != '_')

            return (0);

        i++;

    }

    return (1);

}

 

void remove_var_name(char **envp, char *name_var)

{

    int i;

    int j;



    i = 0; 

    while (envp[i])

    {

        j = 0;

        while (envp[i][j] && envp[i][j] != '=')

            j++;

        if (ft_strncmp(envp[i], name_var, j) == 0 && envp[i][j] == '=')

        {

            free(envp[i]);

            envp[i] = NULL;

            while (envp[i + 1])

            {

                envp[i] = envp[i + 1];

                i++;

            }

            envp[i] = NULL;

            return;

        }

        i++;

    }

    // No hace falta mostrar error si no se encuentra; Bash no lo hace.

}

 

int ft_unset(t_struct *mini, char **args)

{

    int i;

    int status;



    i =1;

    status = 1; // Asume que fue un éxito.

    while (args[i])

    {

        if (!is_valid_var_name(args[i]))

        {

            ft_putstr_fd("unset: `", 2);

            ft_putstr_fd(args[i], 2);

            ft_putendl_fd("` is not a valid identifier", 2);

            status = 0; // Marca que hubo error, pero sigue

        }

        else

            remove_var_name(mini->envp, args[i]);

        i++;

    }

    return (status); // Devuelve 1 si todo bien, 0 si algo falló

}



FX PWD



void ft_pwd(t_struct *mini)
{
    int i;

    i = 0;



    while (mini->envp[i])
    {
        if (ft_strncmp(mini->envp[i], "PWD=", 4) == 0)
        {
            ft_putendl_fd(mini->envp[i] + 4, 1); // Imprime solo el valor de la variable
            return;
        }
        i++;
    }
    ft_putendl_fd("pwd: PWD not set", 2);
}



FX ENV



//para copiar envp del main.

char **copy_envp(char **envp)

{

    int i = 0;

     int j = 0;

    char **copy;

 

    while (envp[i])

        i++;

    copy = malloc(sizeof(char *) * (i + 1));

    if (!copy)

        return NULL;

    while (j < i)

    {

        copy[j] = strdup(envp[j]); // duplicamos cada línea

        if( !copy[j])

        {

              while (j >= 0)

              {

                    free(copy[j]);

                     j--;

               }

               free(copy);

                return NULL;

          }

            j++;

    }

    copy[i] = NULL;

    return (copy);

}

 

 

 //función en sí

void ft_env (t_struct *mini)

{

      int i = 0;

      while (mini->envp[i])

      {

              printf("%s\n", mini->envp[i]);

              i++;

       }

}



FX CD



char *get_env_value(const char *name, char **envp)
{
  size_t name_len;

  name_len = ft_strlen(name);
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


void update_pwd(t_struct *mini, const char *new_path)

{

    char *oldpwd;

    char *newpwd;

    int i;

 

    i = 0;

    while (mini->envp[i] && ft_strncmp(mini->envp[i], "PWD=", 4))

        i++;

 

    if (mini->envp[i])

        oldpwd = ft_strjoin("OLDPWD=", mini->envp[i] + 4);

    else

        oldpwd = ft_strdup("OLDPWD=");

 

    newpwd = ft_strjoin("PWD=", new_path);

 

    add_to_envp(mini, oldpwd);

    add_to_envp(mini, newpwd);

 

    free(oldpwd);

    free(newpwd);

}


 

int is_valid_directory(char *path)
{
    struct stat info;

    // Verifica si la ruta existe y se puede acceder
    if (stat(path, &info) == 0)
    {
        // Verifica si la ruta es un directorio
        if (S_ISDIR(info.st_mode))
            return (1); // Es un directorio válido
    }
    return (0); // No existe o no es un directorio
}

void go_home(t_struct *mini)
{
    int i;
    char *home_path;

    size_t  len;


     home_path = NULL;

     i = 0;
    while (mini->envp[i])
    {
        if (ft_strncmp(mini->envp[i], "HOME=", 5) == 0)
        {
            len = ft_strlen(mini->envp[i]);
            home_path = ft_substr(mini->envp[i], 5, len - 5);
            if (!home_path)
                return;

            chdir(home_path);
            update_pwd(mini, home_path);
            free(home_path);
            return;
        }
        i++;
    }
    ft_putendl_fd("cd: HOME not set", 2);
}

char *ft_get_parent_pwd(t_struct *mini)
{
    char *pwd;
    char *parent;
    int len;

    pwd = get_env_value("PWD", mini->envp);

    if (!pwd)
        return (NULL);
    len = ft_strlen(pwd);
    while (len > 0 && pwd[len] != '/')
        len--;
    if (len == 0)
        return (ft_strdup("/"));
    parent = ft_substr(pwd, 0, len);
    return (parent);
}

int go_to_parent(t_struct *mini)
{
    char *new_dir;

    new_dir = ft_get_parent_pwd(mini);
    if (!new_dir)
        return (1);
    if (is_valid_directory(new_dir))
    {
        chdir(new_dir);
        update_pwd(mini, new_dir);
    }
    else
        ft_putendl_fd("cd: no such directory", 2);
    free(new_dir);
    return (0);
}

int go_to_oldpwd(t_struct *mini)
{
    char *oldpwd = get_env_value("OLDPWD", mini->envp);
    if (!oldpwd)
    {
        ft_putendl_fd("cd: OLDPWD not set", 2);
        return (0);
    }

    char *copy = ft_strdup(oldpwd); // hacemos copia para liberar después
    if (!copy)
        return (0);

    if (is_valid_directory(copy))
    {
        chdir(copy);
        ft_putendl_fd(copy, 1);
        update_pwd(mini, copy);
    }
    else
        ft_putendl_fd("cd: no such directory", 2);

    free(copy);
    return (0);
}

int ft_cd(t_struct *mini, char **arg)
{
    char *path;

    path = arg[1];
    if (!path)
        return (go_home(mini)); // Si no hay argumento, ir al HOME

    if (ft_strncmp(path, "-", 1) == 0 && path[1] == '\0')
        return (go_to_oldpwd(mini)); // Manejo de 'cd -'

    if (path[0] == '/')
    {
        if (is_valid_directory(path))
        {
            chdir(path);
            update_pwd(mini, path);
        }
        else
            ft_putendl_fd("cd: no such directory", 2);
        return (0);
    }

    if (ft_strncmp(path, "..", 2) == 0 && path[2] == '\0')
        return (go_to_parent(mini));

    if (ft_strncmp(path, "./", 2) == 0 || ft_strncmp(path, ".", 1) == 0)
        return (0);

    if (is_valid_directory(path))
    {
        chdir(path);
        update_pwd(mini, path);
    }
    else
        ft_putendl_fd("cd: no such directory", 2);
    return (0);
}



FX EXPORT



int is_valid_var_name(char *argv)

{

    int i;

    i = 0;

    if (!argv || ft_isdigit(argv[i]))

        return (0);

    while (argv[i] && argv[i] != '=')

    {

        if (!ft_isalnum(argv[i]) && argv[i] != '_')

            return (0);

        i++;

    }

    return (1);

}

 

void add_to_envp(t_struct *mini, char *arg)  
{
    int i ;

    int k;
    int name_len; // longitud del nombre de la variable, es decir, hasta el primer =


    i = 0;

    name_len = 0;
    // Calcular longitud del nombre (hasta '=' o fin de string)
    while (arg[name_len] && arg[name_len] != '=')
        name_len++;

    // Buscar si ya existe la variable
    while (mini->envp[i])
    {
        int j = 0;
        while (j < name_len && mini->envp[i][j] == arg[j])
            j++;

        if (j == name_len && mini->envp[i][j] == '=')
        {
            free(mini->envp[i]);
            mini->envp[i] = ft_strdup(arg); // está reemplazando, no añadiendo otra línea.
            return;
        }

        i++;
    }

    // Añadir nueva variable
    char **new_envp = malloc(sizeof(char *) * (i + 2));
    if (!new_envp)
        return;
    k = 0;
    while (k < i)
    {
        new_envp[k] = ft_strdup(mini->envp[k]);

        free(mini->envp[k]);
        k++;
    }
    new_envp[i] = ft_strdup(arg);
    new_envp[i + 1] = NULL;

    free(mini->envp);
    mini->envp = new_envp;
}

 

void ft_export(t_struct *mini, char **arg)
{
    int i;
    char *empty_var;

    i = 0;

    if (!arg || !arg[0])
    {
        ft_env(mini); // Mostrar las variables del entorno si no hay argumentos
        return;
    }

    while (arg[i])
    {
        if (!is_valid_var_name(arg[i]))
        {
            ft_putstr_fd("export: `", 2);
            ft_putstr_fd(arg[i], 2);
            ft_putendl_fd("`: not a valid identifier", 2);
            i++;
            continue;
        }

        // Si no contiene '=', añadir como VAR=
        if (!ft_strchr(arg[i], '='))
        {
            empty_var = ft_strjoin(arg[i], "=");
            if (!empty_var)
                return;
            add_to_envp(mini, empty_var);
            free(empty_var);
        }
        else
            add_to_envp(mini, arg[i]);

        i++;
    }
}



FX ECHO



int is_valid_n_case(char **arg)

{

    int i = 1; // empezamos en 1, saltando el nombre del comando

    int j;

 

    while (arg[i])

    {

        j = 0;

        if (arg[i][j] == '-') // si empieza con guion

        {

            j++;

            if (arg[i][j] != 'n') // si no hay 'n' después del guion, ya no es válido

                break;

 

            while (arg[i][j] == 'n') // seguimos mientras haya 'n'

                j++;

 

            if (arg[i][j] != '\0') // si hay algo más después de las 'n', no es válido

                break;

        }

        else

            break; // si no empieza con '-', salimos

 

        i++; // argumento válido, pasamos al siguiente

    }

 

    return i; // índice del primer argumento que ya no es opción -n válida

}

 

 

void ft_echo(char **arg)

{

    int i = is_valid_n_case(arg);

 

    // Solo imprimimos si hay algo después de las opciones -n

    if (arg[i])

    {

        while (arg[i])

        {

            printf("%s", arg[i]);

            if (arg[i + 1])

                printf(" ");

            i++;

        }

        if (is_valid_n_case(arg) == 1) // no hubo ninguna opción -n válida

            printf("\n");

    }

    else if (is_valid_n_case(arg) == 1)

    {

        // No hay nada que imprimir, pero se necesita salto de línea

        printf("\n");

    }

    // Si solo hubo -n -n ... y nada más, entonces no imprimimos nada


}