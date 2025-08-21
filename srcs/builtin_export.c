#include "../inc/builtins.h"

void	sort_envp(char **envp)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
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

int	is_valid_var_name(char *argv)

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

void	add_to_envp(t_struct *mini, char *arg)
{
	int		i;
	int		k;
	int		j;
	char	**new_envp;

	int name_len; // longitud del nombre de la variable, es decir hasta el primer =
	i = 0;
	name_len = 0;
	// Calcular longitud del nombre (hasta '=' o fin de string)
	while (arg[name_len] && arg[name_len] != '=')
		name_len++;
	// Buscar si ya existe la variable
	while (mini->envp[i])
	{
		j = 0;
		while (j < name_len && mini->envp[i][j] == arg[j])
			j++;
		if (j == name_len && mini->envp[i][j] == '=')
		{
			free(mini->envp[i]);
			mini->envp[i] = ft_strdup(arg); // está reemplazando, no añadiendo otra línea.
			return ;
		}
		i++;
	}
	// Añadir nueva variable
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return ;
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

void	print_export_env(t_struct *mini)
{
	char	**copy;
	int		i;
	char	*equal_sign;

	copy = ft_copy_str_array(mini->envp);
	i = 0;
	if (!copy)
	{
		mini->last_exit_status = 1;
		return ;
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

int	ft_export(t_struct *mini, char **arg)
{
	int		i;
	int		status;
	char	*empty_var;

	i = 0;
	status = 0;
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
