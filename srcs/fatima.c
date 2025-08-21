

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