#include "../inc/builtins.h"

/**
 * @brief Compara dos cadenas para ordenar (para qsort o un bubble sort).
 * Usada para ordenar el entorno para 'export' sin argumentos.
 * @param s1 Puntero a la primera cadena.
 * @param s2 Puntero a la segunda cadena.
 * @return Un entero menor, igual o mayor que cero si s1 es, respectivamente,
 * menor que, igual a o mayor que s2.
 */
static int	compare_strings(const void *s1, const void *s2)
{
	// Los argumentos son punteros a punteros a char (char **).
	// Necesitamos desreferenciarlos para obtener los char *.
	return (ft_strcmp(*(char **)s1, *(char **)s2));
}

/**
 * @brief Ordena un array de strings alfabéticamente.
 * Se usa para la salida de 'export' sin argumentos.
 * @param arr El array de strings a ordenar.
 */
static void	sort_env_array(char **arr)
{
	int	count;

	count = 0;
	while (arr[count])
		count++;
	// qsort es una función estándar de la librería C para ordenar arrays.
	// Necesita el array, el número de elementos, el tamaño de cada elemento,
	// y una función de comparación.
	qsort(arr, count, sizeof(char *), compare_strings);
}

/**

	* @brief Valida si el nombre de una variable es un identificador válido para 'export'.
 * Un nombre válido no puede ser una cadena vacía,
	no puede empezar con un dígito,
 * y solo puede contener caracteres alfanuméricos y guiones bajos ('_').
 * @param arg La cadena completa (KEY o KEY=VALUE) a validar.
 * @return 1 si el nombre de la variable es válido, 0 si no.
 */
static int	is_valid_var_name(char *arg)
{
	int	i;

	i = 0;
	if (!arg || arg[0] == '\0' || arg[0] == '=')
		// No vacío y no empieza con '='
		return (0);
	// El primer carácter no puede ser un dígito
	if (ft_isdigit(arg[0]))
		return (0);
	// Itera hasta encontrar el '=' o el fin de la cadena
	while (arg[i] && arg[i] != '=')
	{
		// Solo permite caracteres alfanuméricos y guiones bajos
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/**

	* @brief Muestra las variables de entorno para el builtin `export` sin argumentos.
 * Imprime las variables ordenadas alfabéticamente y con el formato `declare
	-x`.
 * @param mini Puntero a la estructura principal de la minishell.
 */
static void	print_export_env(t_struct *mini)
{
	char	**sorted_envp;
	int		i;
	char	*equal_sign;

	// 1. Crear una copia del entorno para poder ordenarla sin afectar el original.
	// Usamos ft_copy_str_array de env_utils.h
	sorted_envp = ft_copy_str_array(mini->envp);
	if (!sorted_envp)
	{
		perror("minishell: export: malloc for sorted envp");
		mini->last_exit_status = 1;
		return ;
	}
	// 2. Ordenar la copia del entorno.
	sort_env_array(sorted_envp);
	// 3. Imprimir cada variable con el formato `declare -x`.
	i = 0;
	while (sorted_envp[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		equal_sign = ft_strchr(sorted_envp[i], '=');
		if (equal_sign) // Si la variable tiene un '=', imprimir KEY="VALUE"
		{
			// Imprime la clave (hasta el '=')
			ft_write(STDOUT_FILENO, sorted_envp[i], equal_sign
				- sorted_envp[i]);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			// Imprime el valor (después del '=')
			ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
			ft_putendl_fd("\"", STDOUT_FILENO);
		}
		else 
		{
			ft_putendl_fd(sorted_envp[i], STDOUT_FILENO);
		}
		i++;
	}
	// 4. Liberar la copia ordenada del entorno.
	free_str_array(sorted_envp); // Usamos free_str_array de env_utils.h
}

/**
 * @brief Implementación del builtin 'export'.
 * Añade o actualiza variables de entorno. Si no hay argumentos,
	lista el entorno ordenado.
 * @param mini Puntero a la estructura principal de la minishell.
 * @param args Array de argumentos.
 * @return 0 en éxito, 1 si al menos un argumento no es un identificador válido.
 */
int	ft_export(t_struct *mini, char **args)
{
	int i;
	int status = 0; // Estado de salida: 0 para éxito, 1 para error

	// Si no hay argumentos, imprime el entorno ordenado.
	if (!args || !args[1])
	{
		print_export_env(mini);
		mini->last_exit_status = 0;
		return (0);
	}

	i = 1; // Empieza desde el primer argumento después de "export"
	while (args[i])
	{
		char *key_start = args[i];
		char *value_start = NULL;
		char *equal_sign = ft_strchr(args[i], '=');

		if (!is_valid_var_name(key_start)) // Validar el nombre de la variable
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd("`: not a valid identifier", STDERR_FILENO);
			status = 1; // Marcar error, pero continuar con otros argumentos.
		}
		else
		{
			// Caso: KEY=VALUE
			if (equal_sign)
			{
				// Reemplazar el '=' por '\0' temporalmente para obtener la clave separada
				*equal_sign = '\0';
				value_start = equal_sign + 1;
					// El valor empieza después del '='

				// Usar ft_setenv_var de env_utils.h para actualizar/añadir la variable
				if (ft_setenv_var(mini, key_start, value_start) != 0)
				{
					perror("minishell: export: ft_setenv_var failed");
					status = 1; // Error de asignación de memoria, por ejemplo.
				}
				*equal_sign = '=';
					// Restaurar el '=' en la cadena original (si es necesario para otros usos)
			}
			else
			{
				// Si la variable ya existe, ft_setenv_var la actualizará.
				// Si no existe, la añadirá con un valor vacío ("").
				if (ft_setenv_var(mini, key_start, "") != 0)
				{
					perror("minishell: export: ft_setenv_var failed");
					status = 1;
				}
			}
		}
		i++;
	}
	mini->last_exit_status = status;
		// Actualiza el estado de salida de la shell.
	return (status);
}