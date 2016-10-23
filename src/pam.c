#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include "crypt.h"

static char *const MODULE_NAME = "EpitechPamela";
static t_param *param = NULL;

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int __attribute__((unused)) flags,
                                   int __attribute__((unused)) argc,
                                   const char __attribute__((unused)) **argv)
{
    int retval;
    const char *pass = NULL;

    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);

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

int pam_sm_open_session(pam_handle_t *pamh, int __attribute__((unused)) flags, int __attribute__((unused)) argc,
                        const char __attribute__((unused)) **argv) {
    int retval;
    const char *pass = NULL;
    const char *pUsername;

    retval = pam_get_user(pamh, &pUsername, "Username: ");
    if (retval == PAM_ABORT) {
        return PAM_IGNORE;
    }
    pam_get_data(pamh, MODULE_NAME, (const void **) &pass);
    if (!pass) {
        pam_syslog(pamh, LOG_NOTICE, "%s: Couldn't get password (it is empty)", "");
        //Asking for the password ourselves
        retval = pam_get_authtok(pamh, PAM_AUTHTOK, &pass, "Password for pamela>");
        if (retval != PAM_SUCCESS) {
            pam_syslog(pamh, LOG_ERR, "%s: Prompt for password failed %s",
                       "", pam_strerror(pamh, retval)
            );
            return PAM_IGNORE;
        }
    }
    getFinalPass(pamh, &pass);
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
    param = read_conf(get_real_path("~/pamela.conf", pUsername));
    param->container_path = get_real_path(param->container_path, pUsername);
    param->mount_point = get_real_path(param->mount_point, pUsername);

    if (crypt_file_test(param->container_path) != 0) {
        PUT_DBG(printf("New volume\n"));
        write_urandom(param->container_path, param->container_size);
        if (volume_create(param->container_path, pass, param->device_name) != 0)
            return PAM_IGNORE;
    } else {
        if (crypt_activate_device(param->container_path, pass, param->device_name) != 0) {
            return PAM_IGNORE;
        }
    }

    if (init_device(param->container_path) == NULL) {
        return PAM_IGNORE;
    }
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

const char *getFinalPass(pam_handle_t *pamh, const char **pass) {
    int choice = 0;

    printf("Choose your favorite authentification method for container:\n"
                   "1. Unix password\n"
                   "2. Usb device\n"
                   "3. Unix password + Usb Device\n"
    );

    while (choice < 1 || choice > 3) {
        int result;
        const struct pam_conv *conv;
        result = pam_get_item(pamh, PAM_CONV, (const void **) &conv);
        if (result != PAM_SUCCESS) {
            return *pass;
        }

        struct pam_message message;
        memset(&message, 0, sizeof(message));
        message.msg_style = PAM_PROMPT_ECHO_ON;
        message.msg = "Your choice: ";

        const struct pam_message *msgs[1];
        msgs[0] = &message;


        //Sending the message, asking for password
        struct pam_response *response = NULL;
        memset(&response, 0, sizeof(response));
        result = (conv->conv)(1, msgs, &response, conv->appdata_ptr);
        if (result != PAM_SUCCESS) {
            free(response);
            return *pass;
        }


        //If we got no password, just return;
        if (response[0].resp == NULL) {
            free(response);
            return *pass;
        }

        choice = atoi(response[0].resp);
    }
    printf("You choose : ");
    switch (choice) {
        case 1:
            printf("Unix password %s\n", *pass);
            break;
        case 2:
            printf("Usb Device\n");
            call_shell_script(pamh, pass);
            break;
        case 3:
            printf("Unix password + usb device\n");
            const char *usb_pass;
            char new_pass[128];
            if (call_shell_script(pamh, &usb_pass) != 0) {
                break;
            }
            snprintf(new_pass, 128, "%s:%s", usb_pass, *pass);
            *pass = strdup(new_pass);
            break;
    }
    return (*pass);
}