#pragma once

#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <libcryptsetup.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

#include <stddef.h>

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define GM(x)   ((size_t) (x) << 20)

typedef struct s_param t_param;


struct s_param
{
  const char		*container_path; //Relative or absolute path
  const char		*mount_point; //Relative or absolute path
  size_t		container_size; //in Mo
};

#include "proto.h"