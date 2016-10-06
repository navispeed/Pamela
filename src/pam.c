#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

/*
 *
 */
/* expected hook */
/*
PAM_EXTERN int pam_sm_setcred(pam_handle_t __attribute__((unused)) *pamh, int __attribute__((unused)) flags,
                              int __attribute__((unused)) argc, __attribute__((unused)) const char **argv) {
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t __attribute__((unused)) *pamh, __attribute__((unused)) int flags,
                                __attribute__((unused)) int argc, __attribute__((unused)) const char **argv) {
  printf("Acct mgmt\n");
  return PAM_SUCCESS;
}

*/

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate(__attribute__((unused)) pam_handle_t *pamh, __attribute__((unused)) int flags,
                                   __attribute__((unused)) int argc, __attribute__((unused)) const char **argv) {
  int retval;

  const char *pUsername;
  const char *pass;
  retval = pam_get_user(pamh, &pUsername, "Username: ");
  retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)(const void*)&pass);

  fprintf(stderr, "Welcome %s\n", pUsername);

  
  if (retval != PAM_SUCCESS) {
    return retval;
  }

  if (strcmp(pUsername, "guemas_g") != 0) {
    return PAM_AUTH_ERR;
  }

  return PAM_SUCCESS;
}
