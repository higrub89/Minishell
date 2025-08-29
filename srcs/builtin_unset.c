#include "../inc/builtins.h"

//omitirlo porque lo tiene el export. con el .h se evitaría.

static char	**remove_from_export_list(t_struct *mini, char **export_list, char *var_name) //omitirlo
{
    int i;
	int j;
	int count;
    char **new_list;

	i = 0;
	j = 0;
	count = 0;
    // Contar elementos
	while (export_list && export_list[count])
		count++;
    // Reservar nueva lista
	new_list = malloc(sizeof(char *) * (count + 1));
	if (!new_list)
		return (mini->last_exit_status = 1, export_list); // fallback: no modificar
    // Copiar elementos excepto el que se elimina
	while (export_list[i])
	{
		if (!ft_strcmp(export_list[i], var_name))
			new_list[j++] = ft_strdup(export_list[i]); //duplicar la memoria.
		free(export_list[i]); // liberar el eliminado
		i++;
	}
	new_list[j] = NULL;
	free(export_list);
	return new_list;
}

static void remove_var_from_envp(char **envp, char *name_var)
{
	int	i;
	int	j;

	i = 0;
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
    int	i;

	i = 1;
	if (!args[1])
	{
		mini->last_exit_status = 0;
		return (0);
	}
	while (args[i])
	{
		if (args[i][0] == '-') // ← Detecta opción
		{
			ft_putstr_fd("minishell: unset: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd(": options are not supported", STDERR_FILENO);
			mini->last_exit_status = 1;
		}
		else if (ft_strcmp (args[i], "PATH") == 0) //detecta el nombre variable PATH
		{
			free_str_array(mini->envp);
			mini->envp = malloc(sizeof(char *));
			if (!mini->envp)
    			return (mini->last_exit_status= 1, 1); // Manejo de error
			mini->envp[0] = NULL;
		}
		else
		{
			remove_var_from_envp(mini->envp, args[i]);
			mini->export_list = remove_from_export_list(mini, mini->export_list, args[i]); //para eliminar de export.
		}
		i++;
	}
    // Si no hubo errores, exit_status debe ser 0
	if (mini->last_exit_status != 1)
		mini->last_exit_status = 0;
	return (mini->last_exit_status);
}
