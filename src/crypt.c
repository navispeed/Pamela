#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "crypt.h"
#include <string.h>

#define BS 4096

int crypt_activate_device(const char *path, const char *key,
                          const char *device_name) {
    struct crypt_device *cd;
    struct crypt_active_device cad;
    int r;

    if (crypt_init(&cd, path) < 0) {
        perror("ACTIVATE INIT");
        return (-1);
    }
    if ((r = crypt_load(cd, CRYPT_LUKS1, NULL)) < 0) {
        fprintf(stderr, "crypt_load() failed\n");
        perror("LOAD");
        return (r);
    }
    dump_device(device_name, cd);
    PUT_DBG(printf("\tKey is: %s with length %lu\n", key, strlen(key)));
    if ((r = crypt_activate_by_passphrase(cd, device_name, CRYPT_ANY_SLOT,
                                          key, strlen(key), CRYPT_ACTIVATE_NO_UUID)) < 0) {
        fprintf(stderr, "crypt_activate_by_passphrase() failed\n");
        PUT_DBG(perror("ACTIVATE"));
        return (r);
    }
    printf("LUKS device %s/%s is active.\n", crypt_get_dir(), device_name);
    if ((r = crypt_get_active_device(cd, device_name, &cad)) < 0) {
        fprintf(stderr, "crypt_get_active_device() failed\n");
        PUT_DBG(perror("ACTIVE"));
        return (r);
    }
    return (0);
}

int crypt_file_test(const char *path) {
    struct crypt_device *cd;

    if (crypt_init(&cd, path) < 0) {
        perror("FILE TEST INIT");
        return (-1);
    }
    if ((crypt_load(cd, CRYPT_LUKS1, NULL)) < 0) {
        perror("FILE TEST LOAD");
        return (-1);
    } else {
        PUT_DBG(fprintf(stdout, "File is already formated\n"));
        return (0);
    }
}

int write_urandom(const char *path, size_t size) {
    int fd_in;
    int fd_out;
    size_t readed = 0;
    char buf[BS];

    fd_in = open("/dev/urandom", O_RDONLY);
    fd_out = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd_in == -1 || fd_out == -1) {
        fprintf(stderr, "cannot open %s file", (fd_in == -1 ? "input" : "output"));
        return (-1);
    }
    while (size > 0 && (readed = (size_t) read(fd_in, buf, BS)) > 0) {
        write(fd_out, buf, readed);
        size = (size > readed ? size - readed : readed - size);
    }
    return (0);
}

void mkfs_format(const char *path) {
    char buff[BS];
    snprintf(buff, BS, "mkfs.ext4 %s", path);
    fprintf(stdout, "System(%s)\n", buff);
    system(buff);
}

static __attribute__((unused)) int free_crypt(struct crypt_device *cd) {
    crypt_free(cd);
    return (0);
}

int crypt_volume(struct crypt_device *cd,
                 const char *key) {
    if (crypt_keyslot_add_by_volume_key(cd, CRYPT_ANY_SLOT,
                                        NULL, 0, key, strlen(key)) < 0) {
        fprintf(stderr, "crypt_keyslot_add_by_volume_key() failed\n");
        perror("KEYSLOT ADD BY VOLUME KEY");
        return (-1);
    }
    return (0);
}

void dump_device(const char __attribute__((unused)) *device_name, struct crypt_device __attribute__((unused)) *cd) {
    PUT_DBG(
            printf("LUKS device %s/%s gonna be activate.\n", crypt_get_dir(), device_name);
            printf("\tcipher used: %s\n", crypt_get_cipher(cd));
            printf("\tcipher mode: %s\n", crypt_get_cipher_mode(cd));
            printf("\tdevice UUID: %s\n", crypt_get_uuid(cd));
            printf("\tdevice name: %s\n", crypt_get_device_name(cd));
    )
}

int volume_format(struct crypt_device *cd,
                  struct crypt_params_luks1 params,
                  const char *key,
                  const char *device_name,
                  const char *path) {
    int r;

    if ((crypt_load(cd, CRYPT_LUKS1, NULL)) == 0) {
        return (1);
    }

    if ((r = crypt_format(cd, CRYPT_LUKS1, "aes", "xts-plain64", NULL, NULL,
                          256 / 8, &params)) < 0) {
        fprintf(stderr, "crypt_format() failed\n");
        perror("FORMAT");
        return (r);
    }

    if ((r = crypt_volume(cd, key)) < 0) {
        fprintf(stderr, "crypt_volume() failed\n");
        perror("CRYPT");
        return (r);
    }

    crypt_activate_device(path, key, device_name);

    char path_to_device[BS];

    sprintf(path_to_device, "%s/%s", crypt_get_dir(), device_name);

    mkfs_format(path_to_device);

    printf("FINISH volume format\n");
    return (0);
}

struct crypt_device *init_device(const char *path) {
    struct crypt_device *cd;
    int r;

    if ((r = crypt_init(&cd, path)) < 0)
    {
        fprintf(stderr, "crypt_init failed on path %s with error %d\n", path, r);
        perror("INIT");
        return (NULL);
    }
    return (cd);
}

int volume_create(const char *path, const char *key,
                  const char *device_name) {
    struct crypt_device *cd;
    struct crypt_params_luks1 params;
    int r;

    if (geteuid()) {
        printf("Using of libcryptsetup requires super user privileges.\n");
        return (1);
    }
    cd = init_device(path);
    if (cd == NULL) {
        return -1;
    }
    printf("cd : %p\n", cd);
    printf("Context is attached to the block %s\n", crypt_get_device_name(cd));
    params.hash = "sha1";
    params.data_alignment = 0;
    params.data_device = NULL;
    if ((r = volume_format(cd, params, key, device_name, path)) < 0) {
        fprintf(stderr, "format() failed on path %s with error %d\n", path, r);
        return -1;
    }
    crypt_free(cd);
    return (0);
}

int desactivate_device(const char *device_name) {
    int r;
    struct crypt_device *cd;

    r = crypt_init_by_name(&cd, device_name);
    if (r < 0) {
        printf("crypt_init_by_name() failed for %s.\n", device_name);
        return (r);
    }
    r = crypt_deactivate(cd, device_name);
    if (r < 0) {
        printf("crypt_deactivate() failed.\n");
        crypt_free(cd);
        return (r);
    }
    printf("Device %s is now deactivated.\n", device_name);
    return 0;
}

int volume_mount(const char *device_name, const char *dest) {
    char path_to_device[BS];

    sprintf(path_to_device, "mount %s/%s %s", crypt_get_dir(), device_name, dest);
    system(path_to_device);
    return (0);
}


int volume_umount(const char *device_name) {
    char path_to_device[BS];

    sprintf(path_to_device, "umount %s/%s", crypt_get_dir(), device_name);
    system(path_to_device);
    return (0);
}


int volume_status(const char __attribute__((unused)) *path, const char __attribute__((unused)) *key,
                  const char *device_name) {
    int r;
    struct crypt_device *cd;

    r = crypt_init_by_name(&cd, device_name);
    if (r < 0) {
        printf("crypt_init_by_name() failed for %s.\n", device_name);
        return (r);
    }
    if (crypt_status(cd, device_name) == CRYPT_ACTIVE)
        printf("Device %s is still active.\n", device_name);
    else {
        printf("Something failed perhaps, device %s is not active.\n", device_name);
        crypt_free(cd);
        return (-1);
    }
    crypt_free(cd);
    return (0);
}
