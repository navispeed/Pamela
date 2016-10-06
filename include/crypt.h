#pragma once

#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <stddef.h>

typedef struct s_param t_param;


struct s_param
{
  const char		*container_path; //Relative or absolute path
  const size_t		container_size; //in Mo
};

#include "proto.h"