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
#include <stdbool.h>
#include <crypt.h>

static char *const script = "/usr/bin/list_usb.sh";
static char *const usb_file_tmp = "/tmp/usb_list.tmp";

char *strdup(const char *);

static bool prompt_user_choice(struct pam_handle *pamh) {
    int result;
    const struct pam_conv *conv;

    if (pamh == NULL) {
        return true;
    }

    result = pam_get_item(pamh, PAM_CONV, (const void **) &conv);
    if (result != PAM_SUCCESS) {
        return false;
    }

    struct pam_message message;
    memset(&message, 0, sizeof(message));
    message.msg_style = PAM_PROMPT_ECHO_ON;
    message.msg = ">";

    const struct pam_message *msgs[1];
    msgs[0] = &message;


    //Sending the message, asking for password
    struct pam_response *response = NULL;
    memset(&response, 0, sizeof(response));
    result = (conv->conv)(1, msgs, &response, conv->appdata_ptr);
    if (result != PAM_SUCCESS) {
        free(response);
        return false;
    }


    //If we got no password, just return;
    if (response[0].resp == NULL) {
        free(response);
        return false;
    }

    if (strncmp(response[0].resp, "y", 1) == 0 || strncmp(response[0].resp, "Y", 1) == 0) {
        return true;
    }
    return false;
}

static void choose_usb_device(struct pam_handle *pamh, const char **pass) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(usb_file_tmp, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        char *key = strdup(line);
        char *value = 0;

        value = strstr(key, "=");
        key[value - key] = 0;
        value = value + 1;

        printf("Device: %s\nuse this device ? (Y/n)\n", key);
        if (prompt_user_choice(pamh)) {
            *pass = strdup(value);
            return;
        }
        free(key);
    }
    fclose(fp);
}

int call_shell_script(struct pam_handle *pamh, const char **key) {
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
    choose_usb_device(pamh, key);
    unlink(usb_file_tmp);
    return 0;
}