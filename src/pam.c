#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypt.h"

int pam_sm_open_session(pam_handle_t *pamh, int __attribute__((unused)) flags, int __attribute__((unused)) argc,
                        const char __attribute__((unused)) **argv) {
    int retval;

    const char *pUsername;
    const char *pass;
    const t_param *param;

    retval = pam_get_user(pamh, &pUsername, "Username: ");
    if (retval == PAM_ABORT) {
        return PAM_IGNORE;
    }
    retval = pam_get_authtok(pamh, PAM_AUTHTOK,
                             &pass, NULL);
    printf("user: %s, pass: %s\n", pUsername, pass);
    param = read_conf("~/pamela.conf");

    return PAM_SUCCESS;
}

int pam_close_session(pam_handle_t __attribute__((unused)) *pamh, int __attribute__((unused)) flags) {
    return PAM_SUCCESS;
}