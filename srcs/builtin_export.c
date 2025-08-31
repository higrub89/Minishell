#include "../inc/builtins.h"

int	is_valid_var_name(char *argv)
{
	int i;

	i = 0;
	if (!argv || argv[0] == '=' || ft_isdigit(argv[0]))
		return (0);
	while (argv[i] && argv[i] != '=')
	{
		if (!ft_isalnum(argv[i]) && argv[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_same_var_name(const char *a, const char *b) //ayuda cuando tienes Z=1 y pones export Z. 
{
	int i = 0;

	while (a[i] && b[i] && a[i] != '=' && b[i] != '=')
	{
		if (a[i] != b[i])
			return (0);
		i++;
	}
	// Ambos deben terminar en '=' o '\0' al mismo tiempo
	return ((a[i] == '=' || a[i] == '\0') && (b[i] == '=' || b[i] == '\0'));
}

int	is_in_export_list(char **export_list, char *var)
{
	int i = 0;
	while (export_list && export_list[i])
	{
		if (is_same_var_name(export_list[i], var))
			return (1);
		i++;
	}
	return (0);
}

void	sort_envp(char **envp)
{
	int		i;
	char	*tmp;

	i = 0;
	while (envp[i])
	{
		int j = 0;
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

char	**add_to_export_list(char **export_list, char *var)
{
	int		i;
	int		j;
    char	**new_list;

	i = 0;
	j = 0;
	while (export_list && export_list[i])
		i++;
	new_list = malloc(sizeof(char *) * (i + 2));
	if (!new_list)
		return (export_list);
	while (j < i)
		new_list[j] = ft_strdup(export_list[j]); //importante asignar memoria. Si no pierde la cabeza del puntero.		j++;
	new_list[i] = ft_strdup(var);
	new_list[i + 1] = NULL;
	free_str_array(export_list);
	return (new_list);
}

char	**remove_from_export_list(t_struct *mini, char **export_list, char *var_name)
{
    int i;
	int j;
	int count;
    char **new_list;

	i = 0;
	j = 0;
	count = 0;
	if (!export_list || !var_name)
		return (export_list);
    // Contar elementos
	while (export_list && export_list[count]) // OJO creo que podría usar la función de la libft para reducir líneas
		count++;
    // Reservar nueva lista
	new_list = malloc(sizeof(char *) * (count + 1));
	if (!new_list)
		return (mini->last_exit_status = 1, export_list); // fallback: no modificar
    // Copiar elementos excepto el que se elimina
	while (export_list[i])
	{
		if (ft_strcmp(export_list[i], var_name) != 0)
			new_list[j++] = ft_strdup(export_list[i]); //duplicar la memoria
		i++;
	}
	new_list[j] = NULL;
	free_str_array(export_list);
	return (new_list);
}

void	add_to_envp(t_struct *mini, char *arg)
{
    int 	i;
	int		j;
	int 	k;
	int		name_len;
    char	**new_envp;

	i = 0;
	k = 0;
	name_len = 0;
	while (arg[name_len] && arg[name_len] != '=')
		name_len++;
	while (mini->envp && mini->envp[i])
	{
		j = 0;
		while (j < name_len && mini->envp[i][j] == arg[j])
			j++;
		if (j == name_len && mini->envp[i][j] == '=')
		{
			free(mini->envp[i]);
			mini->envp[i] = ft_strdup(arg);
			return;
		}
		i++;
	}
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
	{
		mini->last_exit_status = 1;
		return ;
	}
	while (k < i)
	{
		new_envp[k] = ft_strdup(mini->envp[k]);
		k++;
	}
	new_envp[i] = ft_strdup(arg);
	new_envp[i + 1] = NULL;
	free_str_array(mini->envp);
	mini->envp = new_envp;
}

void	print_export_env(t_struct *mini)
{
	char	**copy_env;
	char	**copy_export;
	char	*equal_sign;
	int 	i;

	i = 0;
	copy_env = ft_copy_str_array(mini->envp);
	if (!mini->export_list)
	{
    	if (mini->envp)
        	mini->export_list = ft_copy_str_array(mini->envp);
    	else
    	{
        	mini->export_list = malloc(sizeof(char *));
        	if (!mini->export_list)
        	{
            	mini->last_exit_status = 1;
            	return;
        	}
        	mini->export_list[0] = NULL;
    	}
	}
	copy_export = ft_copy_str_array(mini->export_list);
	if (!copy_env || !copy_export)
	{
		mini->last_exit_status = 1;
		return;
	}
	sort_envp(copy_env);
	while (copy_env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		equal_sign = ft_strchr(copy_env[i], '=');
		if (equal_sign)
		{
			write(STDOUT_FILENO, copy_env[i], equal_sign - copy_env[i]);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
			ft_putendl_fd("\"", STDOUT_FILENO); //con esto se arreglo que imprima Z="1" -antes Z="1
		}
		i++;
	}
	free_str_array(copy_env);
	mini->last_exit_status = 0;
}

int ft_export(t_struct *mini, char **arg)
{
    int     i;
    int     status;
    char    *var_name;
    int     name_len;

	i = 1;
	status = 0;
    if (!arg[1]) //arg[0] = export
    {
        print_export_env(mini);
        return (0);
    }
    while (arg[i])
    {
        if (arg[i][0] == '-')
        {
            ft_putstr_fd("minishell: export: `", STDERR_FILENO);
            ft_putstr_fd(arg[i], STDERR_FILENO);
            ft_putendl_fd("`: options are not supported", STDERR_FILENO);
            status = 1;
        }
        else if (!is_valid_var_name(arg[i]))
        {
            ft_putstr_fd("export: `", STDERR_FILENO);
            ft_putstr_fd(arg[i], STDERR_FILENO);
            ft_putendl_fd("`: not a valid identifier", STDERR_FILENO);
            status = 1;
        }
        else if (ft_strchr(arg[i], '='))
        {
            name_len = 0;
            while (arg[i][name_len] && arg[i][name_len] != '=')
                name_len++;
            var_name = ft_substr(arg[i], 0, name_len);
            // Eliminar si ya existe en export_list
            if (is_in_export_list(mini->export_list, var_name)) //para que actualice la variable: VAR luego VAR=hola.
                mini->export_list = remove_from_export_list(mini, mini->export_list, var_name);

            // Añadir nueva versión con valor
            mini->export_list = add_to_export_list(mini->export_list, arg[i]);

            // Actualizar envp
            add_to_envp(mini, arg[i]);
            free(var_name);
        }
        else
        {
            // Solo añadir si no existe
            if (!is_in_export_list(mini->export_list, arg[i]))
                mini->export_list = add_to_export_list(mini->export_list, arg[i]); //solo se guardan en export_list las que no tienen valor (VAR1)
        }
        i++;
    }
    mini->last_exit_status = status;
    return (status);
}