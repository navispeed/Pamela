#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "crypt.h"

static t_param *param = NULL;

int pam_sm_open_session(pam_handle_t *pamh, int __attribute__((unused)) flags, int __attribute__((unused)) argc,
                        const char __attribute__((unused)) **argv) {
    int retval;

    const char *pUsername;
    const char *pass;

    retval = pam_get_user(pamh, &pUsername, "Username: ");
    if (retval == PAM_ABORT) {
        return PAM_IGNORE;
    }
    //TODO change this
    retval = pam_get_authtok(pamh, PAM_AUTHTOK,
                             &pass, "pamela>");
    printf("user: %s, pass: %s\n", pUsername, pass);
    param = read_conf(get_real_path("~/pamela.conf", pUsername));
    param->container_path = get_real_path(param->container_path, pUsername);
    param->mount_point = get_real_path(param->mount_point, pUsername);

    if (crypt_file_test(param->container_path) != 0) {
        printf("New volume\n");
        write_urandom(param->container_path, param->container_size);
        volume_create(param->container_path, pass, param->device_name);
    } else {
        crypt_activate_device(param->container_path, pass, param->device_name);
    }

    init_device(param->container_path);
    printf("init device OK\n");

    mkdir(param->mount_point, 555);

    // volume mount (device name)
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

int pam_sm_close_session(pam_handle_t __attribute__((unused)) *pamh, int __attribute__((unused)) flags, int __attribute__((unused)) argc,
                         const char __attribute__((unused)) **argv) {
    volume_umount(param->device_name);
    desactivate_device(param->device_name);
    return PAM_SUCCESS;
}