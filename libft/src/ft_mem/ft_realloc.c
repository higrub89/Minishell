#include "../../inc/libft.h"

void  *ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
  void *new_ptr;

  if (new_size == 0)
  {
    if (ptr)
      free(ptr);
    return (NULL);
  }
  if (!ptr)
    return (malloc(new_size));
  if (new_size == old_size)
    return (ptr);
  new_ptr = malloc(new_size);
  if (!new_ptr)
    return (NULL);
  if (old_size < new_size)
    ft_memcpy(new_ptr, ptr, old_size);
  else
    ft_memcpy(new_size, ptr, new_size);
  free(ptr);
  return (new_ptr);
}

//Función Realloc, sirve para agregar más tamaña de memoria a los bloques de memoria ya asignados. El proposito de realloc en C es expandir los bloques de memoria actuales dejando el contenido original como está. Tambien ayuda a reducir el tamaño de la memoria previamente asiganda mediante funciones malloc o calloc. Realloc(reasignación de memoria). Le he asignado un argumento adicional (old_size), previamente debemos asignar el size del *ptr.