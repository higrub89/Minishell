    int j;

    i = 1;
    while (arg[i])
    {
        j = 0;MAIN

Está bien, sí hizo la copia no solo apunta a envp. 


ft_echo


int is_valid_n_case(char **arg)
{
    int i;

        if (arg[i][j] == '-')
        {
            j++;
            if (arg[i][j] != 'n')
                break;

            while (arg[i][j] == 'n')
                j++;

            if (arg[i][j] != '\0')
                break;
        }
        else
            break;

        i++;
    }

    return i;
}




int ft_echo(char **arg)
{
    int i = is_valid_n_case(arg);
    int n_index = i;

    if (arg[i])
    {
        while (arg[i])
        {
            printf("%s", arg[i]);
            if (arg[i + 1])
                printf(" ");
            i++;
        }
        if (n_index == 1) // no hubo ninguna opción -n válida
            printf("\n");
    }
    else if (n_index == 1)
        printf("\n");
    return (0);
}


pruebas:


Entrada simulada (char *arg[])	  Salida esperada	    Explicación breve
{"echo", "hola", NULL}	               hola\n	       Sin -n, imprime con salto de línea
{"echo", "-n", "hola", NULL}	       hola	       Opción -n válida, sin salto de línea
{"echo", "-n", "-n", "hola", NULL}     hola	       Múltiples -n, sigue siendo válido
{"echo", "-n", "-n", NULL}	      (nada)	        Solo opciones -n, sin texto, sin salto
{"echo", "-n", "-x", "hola", NULL}    -x hola\n	         -x no es válido, se imprime con salto
{"echo", "-n", "-nx", "hola", NULL}	-nx hola\n	-nx no es válido, se imprime con salto
{"echo", "-n", "hola", "mundo", NULL}	hola mundo	Imprime sin salto, múltiples palabras
{"echo", "-n", "-n", "-n", NULL}	(nada)	        Solo opciones válidas, sin texto
{"echo", "-n", "-n", "-n", "!", NULL}	!	           Imprime sin salto
{"echo", NULL}	                        \n	        Solo el comando, sin argumentos





---------------------

ft_export

void sort_envp(char **envp)
{
    int i = 0;
    int j;
    char *tmp;

    while (envp[i])
    {
        j = 0;
        while (envp[j + 1])
        {
            if (ft_strcmp(envp[j], envp[j + 1]) > 0)
            {
                tmp = envp[j];
                envp[j] = envp[j + 1];
                envp[j + 1] = tmp;
            }
            j++;
        }
        i++;
    }
}



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

 


void print_export_env(t_struct *mini)
{
    char **copy = ft_copy_str_array(mini->envp);
    int i = 0;
    char *equal_sign;

    if (!copy)
    {
        mini->last_exit_status = 1;
        return;
    }

    sort_envp(copy);

    while (copy[i])
    {
        ft_putstr_fd("declare -x ", STDOUT_FILENO);
        equal_sign = ft_strchr(copy[i], '=');
        if (equal_sign)
        {
            write(STDOUT_FILENO, copy[i], equal_sign - copy[i]);
            ft_putstr_fd("=\"", STDOUT_FILENO);
            ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
            ft_putendl_fd("\"", STDOUT_FILENO);
        }
        else
            ft_putendl_fd(copy[i], STDOUT_FILENO);
        i++;
    }

    free_str_array(copy);
    mini->last_exit_status = 0;
}

int ft_export(t_struct *mini, char **arg)
{
    int i = 0;
    int status = 0;
    char *empty_var;

    if (!arg || !arg[0])
    {
        print_export_env(mini);
        return (0);
    }

    while (arg[i])
    {
        if (!is_valid_var_name(arg[i]))
        {
            ft_putstr_fd("export: `", STDERR_FILENO);
            ft_putstr_fd(arg[i], STDERR_FILENO);
            ft_putendl_fd("`: not a valid identifier", STDERR_FILENO);
            status = 1;
        }
        else if (!ft_strchr(arg[i], '='))
        {
            empty_var = ft_strjoin(arg[i], "=");
            if (empty_var)
            {
                add_to_envp(mini, empty_var);
                free(empty_var);
            }
        }
        else
            add_to_envp(mini, arg[i]);

        i++;
    }

    mini->last_exit_status = status;
    return (status);
}

Así como lo has escrito, tu implementación de export está perfectamente estructurada y funcional ✅. Has logrado:

Mantener los nombres de funciones que ya usas (add_to_envp, is_valid_var_name, etc.).

Separar la impresión del entorno en print_export_env, lo que reduce líneas en ft_export.

Ordenar el entorno con sort_envp antes de imprimirlo.

Validar correctamente los nombres de variables.

Añadir variables vacías con VAR= si no hay =, como hace Bash.

Reemplazar o añadir variables nuevas en envp.

Además, el uso de ft_copy_str_array y free_str_array para manejar la copia del entorno está muy bien pensado para evitar modificar el original al imprimir.


¿Modificar mini->envp en ft_export?
✅ Sí puedes modificarlo si estás añadiendo o actualizando variables con add_to_envp. Eso es parte del propósito de export.

❌ No deberías modificarlo solo para imprimirlo ordenado. El orden del entorno no debería cambiar solo porque el usuario ejecuta export sin argumentos.
Si luego haces env, verás el orden original


PRUEBAS:


Comando en Minishell	Comportamiento esperado en Bash 	    Qué verificar en tu implementación
export	     Imprime entorno ordenado con declare -x VAR="value    Se imprime el entorno ordenado                  sin modificar envp

export VAR1=hello	Añade VAR1 con valor hello	VAR1=hello aparece en envp y en export

export VAR2="world"	Añade VAR2 con valor "world"	Se guarda correctamente con comillas en la impresión

export VAR3=123	        Añade VAR3 con valor 123	Se imprime como declare -x VAR3="123"

export VAR4	       Añade VAR4 sin valor → VAR4=	Se imprime como declare -x VAR4=""

export VAR1=updated	Reemplaza valor de VAR1 	VAR1 aparece con nuevo valor, sin duplicados

export 1INVALID=value	Error: identificador inválido	Imprime error y no modifica envp

export =oops	        Error: identificador inválido	Imprime error y no añade nada

export VAR-ERROR=bad	Error: identificador inválido	Imprime error y no modifica envp

export VAR5=ok VAR6=also VAR7	//Añade múltiples variables en una sola línea	//Todas se añaden correctamente, VAR7 con valor vacío

env	                Muestra solo variables con =	Variables sin valor no se imprimen si no tienen =

export VARZ=last VARA=first + export	Imprime entorno ordenado alfabéticamente  //VARA aparece antes que VARZ en la impresión


----------------------------------------------------------------------------

fx pwd 

/**
 * ft_pwd - Imprime el directorio de trabajo actual.
 * @mini: Estructura principal del minishell que contiene envp y estado de salida.
 *
 * Esta función intenta obtener el directorio actual usando getcwd.
 * Si falla, recurre a la variable de entorno PWD como respaldo.
 * Actualiza el estado de salida en mini->last_exit_status.
 */
int ft_pwd(t_struct *mini)
{
    char    *cwd;

    // getcwd(NULL, 0) permite que el sistema asigne memoria automáticamente
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        // Fallback: buscar PWD en envp si getcwd falla
        int i = 0;
        while (mini->envp[i])
        {
            if (ft_strncmp(mini->envp[i], "PWD=", 4) == 0)
            {
                ft_putendl_fd(mini->envp[i] + 4, STDOUT_FILENO);
                mini->last_exit_status = 0;
                return (0);
            }
            i++;
        }
        // Si no se encuentra PWD, mostrar error
        perror("minishell: pwd");
        mini->last_exit_status = 1;
        return (1);
    }
    // Imprime el directorio actual y libera la memoria
    ft_putendl_fd(cwd, STDOUT_FILENO);
    free(cwd);
    mini->last_exit_status = 0;
    return (0);
}


PRUEBAS


🧷 Tipo de prueba	💻 Comando de prueba	✅ R. esperado	🔍 Qué evalúa el jurado
Básica en raíz	         cd / && pwd	         /	       Salida correcta en directorio raíz

Básica en subdirectorio	cd /usr/bin && pwd	/usr/bin	Resolución correcta del path

Comparación con Bash	pwd en mini y bash      Misma salida	Consistencia con comportamiento estándar

Redirección de salida	pwd > salida.txt        Archivo contiene el path actual	// Soporte para redirecciones

Uso en pipeline	       `pwd	               grep usr`	/usr/bin si estás allí	Integración con otros comandos

Sin permisos de lectura	chmod 000 . && pwd     Usa PWD o muestra error	//Fallback a PWD si getcwd falla

Sin variable PWD	unset PWD && pwd	Error con perror	Manejo de entorno incompleto

PWD falso	export PWD="/fake/path" && pwd //  /fake/path si getcwd falla	Uso correcto del respaldo PWD

Entorno con muchas vars	export VAR1=... VAR1000=... && pwd  //Path correcto	/Robustez ante entorno cargado

Memoria	Ejecutar con valgrind ./minishell	Sin fugas	Gestión correcta de memoria (free)


-----------------------------------------------------------------------------

fx CD
****

canonical path = ruta absoluta y real.
fx auxiliares:

go_home(mini) → usa get_env_value("HOME", mini) y getcwd para actualizar PWD.

go_to_oldpwd(mini) → imprime la ruta como Bash y actualiza PWD con getcwd.

go_to_parent(mini) → usa get_env_value("PWD", mini) o getcwd, y recorta hasta el último /.

update_pwd_env(mini, new_path) → actualiza OLDPWD y PWD usando ft_setenv_var.

update_pwd_after_chdir

is_valid_directory(path) → usa stat() para verificar si la ruta es válida.

******

¿Qué contiene struct stat?
Una vez incluida, puedes usarla para acceder a cosas como:

st_mode → tipo de archivo y permisos

st_size → tamaño en bytes

st_mtime → última modificación

st_uid / st_gid → propietario y grupo

st_ino → número de inodo
y más

struct stat info;
if (stat("archivo.txt", &info) == 0)
{
    printf("Tamaño: %ld bytes\n", info.st_size);
}

*******


#include <sys/stat.h>

int is_valid_directory(const char *path)
{
    struct stat info;

    // Verifica si la ruta existe y es un directorio
    if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
        return (1);
    return (0);
}

int update_pwd_after_chdir(t_struct *mini)
{
    char *cwd = getcwd(NULL, 0);

    if (!cwd)
    {
        perror("minishell: cd: getcwd failed");
        mini->last_exit_status = 1;
        return (1);
    }

    update_pwd_env(mini, cwd);
    free(cwd);
    mini->last_exit_status = 0;
    return (0);
}


int go_home(t_struct *mini)
{
    char *home = get_env_value("HOME", mini);

    if (!home)
    {
        ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
        mini->last_exit_status = 1;
        return (1);
    }

    if (chdir(home) != 0)
    {
        perror("minishell: cd");
        mini->last_exit_status = 1;
        return (1);
    }

    return (update_pwd_after_chdir(mini));
}


int go_to_oldpwd(t_struct *mini)
{
    char *oldpwd = get_env_value("OLDPWD", mini);

    if (!oldpwd)
    {
        ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
        mini->last_exit_status = 1;
        return (1);
    }

    if (chdir(oldpwd) != 0)
    {
        perror("minishell: cd");
        mini->last_exit_status = 1;
        return (1);
    }
    ft_putendl_fd(oldpwd, STDOUT_FILENO); //imprimir el oldpwd antes del return para imitar bash
    return (update_pwd_after_chdir(mini));
}

int go_to_parent(t_struct *mini)
{
    char *cwd = getcwd(NULL, 0);
    char *parent;

    if (!cwd)
    {
        perror("minishell: cd: getcwd failed");
        mini->last_exit_status = 1;
        return (1);
    }

    parent = ft_strrchr(cwd, '/');
    if (parent && parent != cwd)
        *parent = '\0';
    else
        ft_strcpy(cwd, "/");

    return (update_pwd_after_chdir(mini));
}

void update_pwd_env(t_struct *mini, const char *new_path)
{
    char *oldpwd = get_env_value("PWD", mini);

    if (oldpwd)
        ft_setenv_var(mini, "OLDPWD", oldpwd);
    ft_setenv_var(mini, "PWD", new_path);
}


int ft_cd(t_struct *mini, char **args)
{
    char *target_path = args[1];

    // Caso 1: 'cd' sin argumentos o 'cd ""'
    if (!target_path || target_path[0] == '\0')
        return (go_home(mini));

    // Caso 2: 'cd -'
    if (ft_strncmp(target_path, "-", 2) == 0)
        return (go_to_oldpwd(mini));

    // Caso 3: 'cd ..'
    if (ft_strncmp(target_path, "..", 3) == 0)
        return (go_to_parent(mini));

    // Caso 4: 'cd .' o 'cd ./'
    if (ft_strncmp(target_path, ".", 2) == 0 || ft_strncmp(target_path, "./", 3) == 0)
    {
        char *current_cwd = getcwd(NULL, 0);
        if (current_cwd)
        {
            update_pwd_env(mini, current_cwd);
            free(current_cwd);
            mini->last_exit_status = 0;
            return (0);
        }
        perror("minishell: cd: getcwd failed");
        mini->last_exit_status = 1;
        return (1);
    }

    // Caso 5: ruta absoluta o relativa
    if (!is_valid_directory(target_path))
    {
        perror("minishell: cd");
        mini->last_exit_status = 1;
        return (1);
    }

    if (chdir(target_path) == 0)
        return (update_pwd_after_chdir(mini));

    perror("minishell: cd");
    mini->last_exit_status = 1;
    return (1);
}




chdir() es una función de la biblioteca estándar de C que sirve para cambiar el directorio de trabajo actual de tu proceso. Es como el equivalente en código de hacer cd en la terminal.

getcwd (get current working directory) es una función del sistema que devuelve la ruta absoluta del directorio actual del proceso. No depende de ninguna variable de entorno como PWD, ni del envp del main. Es decir:

📍 Consulta directamente el sistema operativo.

📂 Te dice exactamente dónde estás, incluso si el entorno está mal configurado.

🧼 Devuelve una ruta canónica, resolviendo . y .. automáticamente.

PRUEBAS

Comando	                   Resultado esperado
cd	                Va a HOME, actualiza PWD
cd -	                Va a OLDPWD, imprime la ruta
cd ..	                Va al padre, PWD se actualiza correctamente
cd .	                No cambia de dir, pero actualiza PWD
cd /tmp	                Va a /tmp, PWD = /tmp
cd nonexistent	        Error, no cambia PWD
unset PWD + cd ..	getcwd reconstruye PWD correctamente

-------------------------------------------------------------------------------

fx env

int ft_env(t_struct *mini)
{
    int i = 0;

    while (mini->envp[i])
    {
        // Solo imprime variables que contienen '='
        if (ft_strchr(mini->envp[i], '='))
            ft_putendl_fd(mini->envp[i], STDOUT_FILENO);
        i++;
    }
    mini->last_exit_status = 0;
    return (0);
}


PRUEBAS


🧷 Tipo de prueba	💻 Comando de prueba	✅ Resultado esperado	🔍 Qué evalúa el jurado
Básica	                env	  Lista completa de variables con =	Función básica funcionando correctamente

Variable sin valor	export MYVAR && env	No se muestra MYVAR	Filtrado correcto de variables sin =

Comparación con Bash	env en minishell y en Bash     Misma salida	 Consistencia con comportamiento estándar

Redirección de salida	env > vars.txt	        Archivo contiene las variables	  //Soporte para redirecciones

Uso en pipeline	        `env	              grep PATH`	Muestra la variable PATH	Integración con otros comandos

      se refiere a : env | grep PATH  debe imprimir PATH=/usr/…..


Entorno vacío	    Ejecutar minishell con env -i ./minishell   No imprime nada, pero retorna 0	                                                                                  //                                                                  //Manejo correcto de entorno vacío


Variable modificada	export PATH="/nuevo/path" && env	PATH actualizado visible	  /      /Reflejo de cambios en el entorno

Memoria	                Ejecutar con valgrind ./minishell	Sin fugas	Gestión correcta de memoria (free)

Variable con múltiples =	export STR="a=b=c" && env	STR=a=b=c aparece completo	Manejo correcto de valores con = internos



-------------------------------------------------------------------------------

fx unset


int is_valid_var_name(char *argv)
{
    int i = 0;

    if (!argv || argv[0] == '\0' || ft_isdigit(argv[0])) //Aunque !argv cubre el caso de puntero nulo, también conviene verificar que el string no esté vacío
        return (0);

    while (argv[i])
    {
        if (!ft_isalnum(argv[i]) && argv[i] != '_')
            return (0);
        i++;
    }
    return (1);
}


static void remove_var_from_envp(char **envp, char *name_var)
{
    int i = 0;
    int j;

    while (envp[i])
    {
        j = 0;
        while (envp[i][j] && envp[i][j] != '=')
            j++;

        if (ft_strncmp(envp[i], name_var, j) == 0 && envp[i][j] == '=')
        {
            free(envp[i]);
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
}

int ft_unset(t_struct *mini, char **args)
{
    int i = 1;

    // Si no hay argumentos, unset no hace nada pero devuelve éxito
    if (!args[1])
    {
        mini->last_exit_status = 0;
        return (0);
    }

    while (args[i])
    {
        // Validamos si el nombre de la variable es correcto
        if (is_valid_var_name(args[i]))
            remove_var_from_envp(mini->envp, args[i]);
        // Bash no muestra error si el nombre es inválido, pero puedes marcar fallo si lo deseas
        else
        {
            // Si prefieres seguir el comportamiento de Bash, puedes comentar esta línea
            ft_putendl_fd("unset: not a valid identifier", STDERR_FILENO);
            mini->last_exit_status = 1;
        }
        i++;
    }

    // Si no hubo errores, exit_status debe ser 0
    if (mini->last_exit_status != 1)
        mini->last_exit_status = 0;

    return (mini->last_exit_status);
}


PRUEBAS


Caso de prueba 	                        Comando en Bash	     Esperado	  Verificación en mini
Eliminar variable existente//   export TEST_VAR=hello | unset TEST_VAR	//$TEST_VAR no imprime nada<br>env no muestra TEST_VAR	                                       //env no debe mostrar TEST_VAR


Eliminar variable inexistente	unset NON_EXISTENT_VAR	/No imprime error<br>No cambia nada	/No debe fallar ni mostrar error

Nombre inválido (empieza por número)	unset 1INVALID	No elimina nada<br>No imprime error    // 	                                                            is_valid_var_name debe devolver 0

Nombre inválido (caracter no permitido)	unset VAR-NAME	No elimina nada<br>No imprime error       //	                                                            is_valid_var_name debe devolver 0


Nombre vacío	unset ""	No elimina nada|No imprime error  //is_valid_var_name debe devolver 0

Eliminar múltiples variables/   export A=1 B=2 C=3 | unset A B	/env solo muestra C	/env debe mostrar solo C


Entorno limpio tras varios unset   Varios unset seguidos	env sigue funcionando|No hay duplicados ni errores de memoria	                   //Verificar con env y herramientas como valgrind



-------------------------------------------------------------------------------

fx exit

Se mantiene la que puso él creo y ya porque funciona aunque fue de la IA.

static int is_numeric(char *str)
{
    int i = 0;
    if (!str || str[0] == '\0')
        return (0);
    // Permitir signos '+' o '-' al principio
    if (str[0] == '-' || str[0] == '+')
        i++;
    // Verificar que el resto de caracteres sean dígitos
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
            return (0); // No es un dígito
        i++;
    }
    // Si la cadena solo contenía el signo, tampoco es un número válido ("-", "+")
    if (i == 1 && (str[0] == '-' || str[0] == '+'))
        return (0);
    return (1);
}


static long long	safe_atoll(const char *str)
{
	int			sign;
	long long	result;
	size_t		i;

	sign = 1;
	result = 0;
	i = 0;

	// Saltar espacios iniciales
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;

	// Signo opcional
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}

	// Convertir dígitos manualmente
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}

	return (result * sign);
}

/**
 * @brief Implementación del builtin 'exit'.
 * Termina la minishell con un estado de salida específico.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos (args[0] es "exit", args[1] es el estado de salida opcional).
 * @return Nunca debería retornar, ya que llama a exit().
 */


int	ft_exit(t_struct *mini, char **args)
{
	long long	exit_code;
	int			num_args;

	ft_putendl_fd("exit", STDOUT_FILENO);

	num_args = 0;
	while (args[num_args])
		num_args++;

	if (num_args == 1)
		exit(mini->last_exit_status);
	else if (num_args >= 2)
	{
		if (!is_numeric(args[1]))
		{
			ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
			ft_putstr_fd(args[1], STDERR_FILENO);
			ft_putendl_fd(": numeric argument required", STDERR_FILENO);
			exit(255);
		}
		else if (num_args > 2)
		{
			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
			mini->last_exit_status = 1;
			return (1);
		}
		else
		{
			exit_code = safe_atoll(args[1]);
			exit((unsigned char)exit_code);
		}
	}
	return (0);
}


PRUEBAS


Comando en minishell	Salida esperada en consola	¿Termina el programa?	Código de salida
exit	                   	 exit			Sí			Último $?
exit 42				exit			Sí			42
exit hola		exit<br>minishell: exit: hola: numeric argument required/ Sí	/255

exit 1 2	exit  (mensaje err)minishell: exit: too many arguments / No	        1 (retorno)

exit +7	 			exit			Sí			7
exit -3				exit			 Sí			253 (-3 % 256)
exit -			exit (msj err) minishell: exit: -: numeric argument required	Sí	/255
exit " 8"		exit				Sí			8
exit 9223372036854775808	exit			 Sí			0 (por overflow)

exit ""			exit (msj err)minishell: exit: : numeric argument required	Sí	/255