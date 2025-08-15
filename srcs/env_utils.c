#include "../inc/env_utils.h"

/**
 * @brief Realiza una copia profunda de un array de strings (char **).
 * * @param arr El array de strings original a copiar.

	* @return Un nuevo array de strings o NULL en caso de error de asignación de memoria.
 */
char	**ft_copy_str_array(char **arr)
{
	int		i;
	int		count;
	char	**new_arr;

	if (!arr)
		return (NULL);
	count = 0;
	while (arr[count]) // Contar numero de strings.
		count++;
	new_arr = (char **)malloc(sizeof(char *) * (count + 1));
	if (!new_arr)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_arr[i] = ft_strdup(arr[i]);
		if (!new_arr[i])
		{
			while (--i >= 0)
				free(new_arr[i]);
			free(new_arr);
			return (NULL);
		}
		i++;
	}
	new_arr[count] = NULL;
	return (new_arr);
}

void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

#include "../inc/env_utils.h"

// ... (tus funciones ft_copy_str_array, free_str_array) ...

// LA ÚNICA Y COMPLETA IMPLEMENTACIÓN DE get_env_value
char	*get_env_value(const char *name, t_struct *mini)
{
	int i;
	size_t len;

	char *str_status; // Variable temporal para ft_itoa
	if (!name || !mini || !mini->envp)
		return (ft_strdup(""));
	if (ft_strncmp(name, "?", 2) == 0)
	{
		str_status = ft_itoa(mini->last_exit_status);
		if (!str_status)            // Manejo de error de malloc para ft_itoa
			return (ft_strdup("")); // Devuelve cadena vacía si ft_itoa falla
		return (str_status);        // ft_itoa devuelve una cadena malloc-eada
	}
	 len = ft_strlen(name);
	 i = 0;
	 while (mini->envp[i])
	{
		if (ft_strncmp(mini->envp[i], name, len) == 0 && mini->envp[i][len] == '=')
			return (ft_strdup(mini->envp[i] + len + 1));
		i++;
	}
	return (ft_strdup("")); // Variable no encontrada, retorna cadena vacía
}

int	ft_setenv_var(t_struct *mini, const char *key, const char *value)
{
	char **new_envp;
	char *new_var_str;
	size_t key_len = ft_strlen(key);
	int i = 0;
	int found = 0;

	// 1. Construir la nueva cadena "KEY=VALUE"
	char *eq_key = ft_strjoin(key, "=");
	if (!eq_key)
		return (1); // Fallo de malloc
	new_var_str = ft_strjoin(eq_key, value);
	free(eq_key); // Liberar el string temporal "KEY="
	if (!new_var_str)
		return (1); // Fallo de malloc

	// 2. Buscar si la variable ya existe en el entorno actual
	while (mini->envp[i])
	{
		if (ft_strncmp(mini->envp[i], key, key_len) == 0
			&& mini->envp[i][key_len] == '=')
		{
			// Variable encontrada, actualizar su valor y liberar el antiguo
			free(mini->envp[i]);         // Liberar la string antigua
			mini->envp[i] = new_var_str; // Asignar la nueva string
			found = 1;
			break ;
		}
		i++;
	}

	// 3. Si la variable no fue encontrada, añadirla al final
	if (!found)
	{
		int count = 0;
		while (mini->envp[count]) // Contar cuántas variables hay actualmente
			count++;

		// Asignar un nuevo array de punteros con espacio para la nueva variable y el NULL final
		new_envp = (char **)malloc(sizeof(char *) * (count + 2));
		if (!new_envp)
		{
			free(new_var_str); // Si falla, liberar la string que creamos
			return (1);
		}

		// Copiar los punteros del array antiguo al nuevo (¡no hacemos strdup aquí!)
		// Simplemente re-parenting de las strings existentes.
		i = 0;
		while (mini->envp[i])
		{
			new_envp[i] = mini->envp[i];
			i++;
		}
		new_envp[i] = new_var_str; // Añadir la nueva variable string
		new_envp[i + 1] = NULL;    // Terminar el nuevo array con NULL

		free(mini->envp);
		// Liberar el array viejo de punteros (NO las strings dentro)
		mini->envp = new_envp; // Asignar el nuevo array a mini->envp
	}
	return (0); // Éxito
}