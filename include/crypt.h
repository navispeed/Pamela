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
  size_t		container_size; //in Mo
};

#define GET_AS(jobj, param, member_name, type) \
    type = json_object_get_type(val); \
    switch (type) { \
        case json_type_string: printf("type: json_type_string, "); \
printf("value: %sn", json_object_get_string(val)); \
break; \

#include "proto.h"