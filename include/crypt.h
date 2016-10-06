#pragma once

#include <stddef.h>

typedef struct s_param t_param;


struct s_param
{
  const char		*container_path; //Relative or absolute path
  const size_t		container_size; //in Mo
};
