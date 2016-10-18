#pragma once

#define PAM_SM_PASSWORD
#define PAM_SM_SESSION
#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT

#ifdef DBG
#define PUT_DBG(code) code
#else
#define PUT_DBG(code)
#endif

#include <libcryptsetup.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

#include <stddef.h>

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <pwd.h>



#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define GM(x)   ((size_t) (x) << 20)

typedef struct s_param t_param;


struct s_param
{
  const char		*container_path; //Relative or absolute path
  const char		*mount_point; //Relative or absolute path
  const char		*device_name; //Relative or absolute path
  size_t		container_size; //in Mo
};



#include "proto.h"