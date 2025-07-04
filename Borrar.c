#include <stdio.h>
#include <stdlib.h>

int main()
{
  char *path_value = getenv("");
  if (path_value != NULL)
  {
    printf("El valor de HOME: %s\n", path_value);
  }
  else
  {
    printf("La variable HOME no está definida: %s\n", path_value);
  }
}