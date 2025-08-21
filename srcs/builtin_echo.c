#include "../inc/builtins.h" // Incluye el header de builtins


int is_valid_n_case(char **arg)
{
	int	i;
	int	j;

	i = 1;
	while (arg[i])
	{
		j = 0;
		if (arg[i][j] == '-')
		{
			j++;
			if (arg[i][j] != 'n')
				break ;
			while (arg[i][j] == 'n')
				j++;
			if (arg[i][j] != '\0')
				break ;
		}
		else
			break ;
		i++;
	}
	return (i);
}

int	ft_echo(char **arg)
{
	int	i;
	int	n_index;

	i = is_valid_n_case(arg);
	n_index = i;
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
