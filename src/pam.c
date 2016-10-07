#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypt.h"

int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int retval;

    const char *pUsername;
    const char *pass;

    retval = pam_get_user(pamh, &pUsername, "Username: ");
    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **) (const void *) &pass);

    return PAM_SUCCESS;
}

int pam_close_session(pam_handle_t *pamh, int flags) {
    return PAM_SUCCESS;
}