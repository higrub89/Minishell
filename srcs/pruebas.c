#include "../inc/minishell.h"

int main()
{
    char *s = "Hola";
    char *c = ft_strdup(s);
    ft_printf("Word = %s\nEsto es una copia = %s\n", s,c);
    return (0);
}
