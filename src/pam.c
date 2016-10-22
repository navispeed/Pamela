#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include "crypt.h"
#include <string.h>

static char *const MODULE_NAME = "EpitechPamela";
static t_param *param = NULL;

char *strdup(const char *);

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int __attribute__((unused)) flags,
                                   int __attribute__((unused)) argc,
                                   const char __attribute__((unused)) **argv) {
    int retval;
    const char *pass = NULL;

    printf("YOLO\n");
    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **) &pass);

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
    return PAM_SUCCESS;
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

    printf("read conf\n");
    param = read_conf(get_real_path("~/pamela.conf", pUsername));
    param->container_path = get_real_path(param->container_path, pUsername);
    param->mount_point = get_real_path(param->mount_point, pUsername);

    pass = getFinalPass(&pass);
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

const char *getFinalPass(const char **pass) {
    int choice = 0;

    printf("Choose your favorite authentification method for container:\n"
                   "1. Unix password\n"
                   "2. Usb device\n"
                   "3. Unix password + Usb Device"
    );

    while (choice < 1 && choice > 3) {
        scanf("%d", &choice);
    }
    printf("You choose : ");
    switch (choice) {
        case 1:
            printf("Unix password\n");
            break;
        case 2:
            printf("Usb Device\n");
            call_shell_script(pass);
            break;
        case 3:
            printf("Unix password + usb device\n");
            const char *usb_pass;
            char new_pass[128];
            if (call_shell_script(&usb_pass) != 0) {
                break;
            }
            snprintf(new_pass, 128, "%s:%s", usb_pass, *pass);
            *pass = strdup(new_pass);
            break;
    }
    return (*pass);
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