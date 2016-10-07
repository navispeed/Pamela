//
// Created by greg on 07/10/16.
//

#include <stdbool.h>
#include <sys/stat.h>
#include <malloc.h>

static struct stat *get_file(const char *path)
{
    struct stat *s = calloc(1, sizeof(struct stat));

    stat(path, s);
    return s;
}

bool file_exist(const char *path)
{

}

bool folder_exist(const char *path)
{

}

bool create_folder(const char *path)
{

}