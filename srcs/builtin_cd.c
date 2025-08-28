#include "../inc/builtins.h"

int	update_pwd_after_chdir(t_struct *mini);
int	go_home(t_struct *mini);
int	go_to_oldpwd(t_struct *mini);
int	go_to_parent(t_struct *mini);
void	update_pwd_env(t_struct *mini, const char *new_path);

int	is_valid_directory(const char *path)
{
	struct stat	info;

	if (stat(path, &info) != 0)
		return (-1); // No existe
	// Verifica si la ruta existe y es un directorio
	if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
		return (1);
	return (0);
}

int	update_pwd_after_chdir(t_struct *mini)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
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

int	go_home(t_struct *mini)
{
	char	*home;

	home = get_env_value("HOME", mini);
	if (!home)
	{
		ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
		free(home);
		mini->last_exit_status = 1;
		return (1);
	}
	if (chdir(home) != 0)
	{
		perror("minishell: cd");
		free(home);
		mini->last_exit_status = 1;
		return (1);
	}
	return (update_pwd_after_chdir(mini));
}

int	go_to_oldpwd(t_struct *mini)
{
	char	*oldpwd;

	oldpwd = get_env_value("OLDPWD", mini);
	if (!oldpwd)
	{
		ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
		free(oldpwd);
		mini->last_exit_status = 1;
		return (1);
	}
	if (chdir(oldpwd) != 0)
	{
		perror("minishell: cd");
		free(oldpwd);
		mini->last_exit_status = 1;
		return (1);
	}
	ft_putendl_fd(oldpwd, STDOUT_FILENO); //imprimir el oldpwd antes del return para imitar bash
	return (update_pwd_after_chdir(mini));
}

int	go_to_parent(t_struct *mini)
{
	char	*cwd;
	char	*parent;

	cwd = getcwd(NULL, 0);
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
    // Cambiar al directorio padre
	if (chdir(cwd) != 0)
	{
		perror("minishell: cd");
		free(cwd);
		mini->last_exit_status = 1;
		return (1);
	}
	return (update_pwd_after_chdir(mini));
}

void	update_pwd_env(t_struct *mini, const char *new_path)
{
    char	*oldpwd;

	oldpwd = get_env_value("PWD", mini);
	if (oldpwd)
	{
		ft_setenv_var(mini, "OLDPWD", oldpwd);
		free(oldpwd);
	}
	ft_setenv_var(mini, "PWD", new_path);
}

int	ft_cd(t_struct *mini, char **args)
{
	char	*target_path;
	char	*current_cwd;
	int		verification;

	target_path = args[1];
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
		current_cwd = getcwd(NULL, 0);
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
	verification = is_valid_directory(target_path);
    // Caso 5: ruta absoluta o relativa
	if (verification == -1)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(target_path, STDERR_FILENO);
		ft_putendl_fd(": No such file or directory", STDERR_FILENO);
		mini->last_exit_status = 1;
		return (1);
	}
	else if (verification == 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(target_path, STDERR_FILENO);
		ft_putendl_fd(": Not a directory", STDERR_FILENO);
		mini->last_exit_status = 1;
		return (1);
	}
	if (chdir(target_path) == 0)
		return (update_pwd_after_chdir(mini));
	perror("minishell: cd");
	mini->last_exit_status = 1;
	return (1);
}
