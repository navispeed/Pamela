//
// Created by greg on 22/10/16.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>

static char *const script = "/usr/bin/list_usb.sh";
static char *const usb_file_tmp = "/tmp/usb_list.tmp";

int call_shell_script(const char **key)
{
    char buff[128];
    if (access(script, F_OK) != 0) {
        fprintf(stderr, "Cannot access %s, check your installation\n", script);
        return -1;
    }
    snprintf(buff, 128, "%s %s", script, usb_file_tmp);
    if (system(buff) != 0) {
        fprintf(stderr, "Something hapen during usb configuration, aborting\n");
        return -2;
    }
    int fd = open(usb_file_tmp, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Something hapen during usb configuration, aborting\n");
        return -3;
    }
    memset(buff, 0, sizeof(buff));
    ssize_t readed = read(fd, buff, 128);
    if (readed == -1) {
        return -5;
    }
    *key = strdup(buff);
    close(fd);
    unlink(usb_file_tmp);
    return 0;
}