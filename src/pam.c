#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include "crypt.h"

static char *const MODULE_NAME = "esteban";
static t_param *param = NULL;

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    int retval;
    const char *pass = NULL;

    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);
    printf("pam_sm_authenticat %s\n", pass);

    if (retval != PAM_SUCCESS) {
        return PAM_IGNORE;
    }

    size_t i = strlen(pass);

    char *data = malloc(i + 1);

    strcpy(data, pass);

    retval = pam_set_data(pamh, MODULE_NAME, data, NULL);
    if (retval != PAM_SUCCESS) {
        printf("PROBLEME\n");
        return PAM_IGNORE;
    }

    const void *tmp = NULL;
    pam_get_data(pamh, MODULE_NAME, &tmp);
    return PAM_IGNORE;
}

/**
 * Code from kwallet
 * @param pamh
 * @return
 */

static int prompt_for_password(pam_handle_t *pamh)
{
    int result;

    //Get the function we have to call
    const struct pam_conv *conv;
    result = pam_get_item(pamh, PAM_CONV, (const void**)&conv);
    if (result != PAM_SUCCESS) {
        return result;
    }

    //prepare the message
    struct pam_message message;
    memset (&message, 0, sizeof(message));
    message.msg_style = PAM_PROMPT_ECHO_OFF;
    message.msg = "Password: ";

    //We only need one message, but we still have to send it in an array
    const struct pam_message *msgs[1];
    msgs[0] = &message;


    //Sending the message, asking for password
    struct pam_response *response = NULL;
    memset (&response, 0, sizeof(response));
    result = (conv->conv) (1, msgs, &response, conv->appdata_ptr);
    if (result != PAM_SUCCESS) {
        free(response);
        return result;
    }

    //If we got no password, just return;
    if (response[0].resp == NULL) {
        result = PAM_CONV_ERR;
        free(response);
        return result;
    }

    //Set the password in PAM memory
    char *password = response[0].resp;
    result = pam_set_item(pamh, PAM_AUTHTOK, password);

    if (result != PAM_SUCCESS) {
        free(response);
        return result;
    }
    free(response);
    return result;
}

int pam_sm_open_session(pam_handle_t *pamh, int __attribute__((unused)) flags, int __attribute__((unused)) argc,
                        const char __attribute__((unused)) **argv) {
    int retval;
    const char *pass = NULL;
    const char *pUsername;

    printf("pam_sm_open_session\n");
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    retval = pam_get_user(pamh, &pUsername, "Username: ");
    if (retval == PAM_ABORT) {
        return PAM_IGNORE;
    }
    pam_get_data(pamh, MODULE_NAME, (const void **) &pass);
    if (!pass) {
        pam_syslog(pamh, LOG_NOTICE, "%s: Couldn't get password (it is empty)", "");
        //Asking for the password ourselves
        retval = prompt_for_password(pamh);
        if (retval != PAM_SUCCESS) {
            pam_syslog(pamh, LOG_ERR, "%s: Prompt for password failed %s",
                       "", pam_strerror(pamh, retval)
            );
            return PAM_IGNORE;
        }
    }
    if (!pass) {
        fprintf(stderr, "aborted due to null pass\n");
        return PAM_IGNORE;
    }
    switch (retval) {
        case PAM_AUTH_ERR:
            printf("PAM_AUTH_ERR\n");
            break;
        case PAM_AUTHTOK_ERR:
            printf("PAM_AUTHTOK_ERR\n");
            break;
        case PAM_SYSTEM_ERR:
            printf("PAM_SYSTEM_ERR\n");
            break;
        case PAM_TRY_AGAIN:
            printf("PAM_TRY_AGAIN\n");
            break;
    }
    if (retval != PAM_SUCCESS) {
        return PAM_IGNORE;
    }
    printf("user: %s, pass: %s\n", pUsername, pass);
    param = read_conf(get_real_path("~/pamela.conf", pUsername));
    param->container_path = get_real_path(param->container_path, pUsername);
    param->mount_point = get_real_path(param->mount_point, pUsername);

    printf("access : %d\n", access(param->container_path, F_OK) == 0);

    if (crypt_file_test(param->container_path) != 0) {
        PUT_DBG(printf("New volume\n"));
        write_urandom(param->container_path, param->container_size);
        volume_create(param->container_path, pass, param->device_name);
    } else {
        crypt_activate_device(param->container_path, pass, param->device_name);
    }

    init_device(param->container_path);
    PUT_DBG(printf("Init device ok\n"));

    mkdir(param->mount_point, 555);
    volume_mount(param->device_name, param->mount_point);

    struct passwd *owner = getpwnam(pUsername);
    if (owner == NULL) {
        perror("getpwName");
        return PAM_IGNORE;
    }
    chown(param->mount_point, owner->pw_uid, owner->pw_gid);
    printf("volume mount OK\n");

    return PAM_SUCCESS;
}

int pam_sm_close_session(pam_handle_t __attribute__((unused)) *pamh, int __attribute__((unused)) flags,
                         int __attribute__((unused)) argc,
                         const char __attribute__((unused)) **argv) {
    if (param == NULL) {
        return PAM_IGNORE;
    }
    volume_umount(param->device_name);
    desactivate_device(param->device_name);
    return PAM_SUCCESS;
}