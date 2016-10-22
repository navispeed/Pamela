//
// Created by greg on 07/10/16.
//

#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <crypt.h>

const char *get_user_home(const char *username)
{
    const char    *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        struct passwd *pPasswd = getpwnam(username);
        if (pPasswd == NULL) {
            return NULL;
        }
        homedir = pPasswd->pw_dir;
        return homedir;
    }
    return homedir;
}

const char *get_real_path(const char *path, const char *username)
{
    if (path[0] == '~') {
        char *str = malloc(1024);
        sprintf(str, "%s%s", get_user_home(username), path + 1);
        return str;
    }
    return path;
}