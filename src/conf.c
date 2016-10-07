#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include "crypt.h"


static struct {
    char *name;
    size_t shift;
} strToMember[] = {
        {"container_path", 0},
        {"container_size", 8},
        {NULL,             0}
};

static char *read_whole_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return (NULL);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return (string);
}

static const char *find_string(json_object *jobj, const char *member_name) {
    json_object_object_foreach(jobj, key, val) {
        int i = 0;
        while (strToMember[i].name) {
            if (strcmp(key, member_name) == 0) {
                return (json_object_get_string(val));
            }
            ++i;
        }
    }
    return NULL;
}

static long find_int(json_object *jobj, const char *member_name) {
    json_object_object_foreach(jobj, key, val) {
        int i = 0;
        while (strToMember[i].name) {
            if (strcmp(key, member_name) == 0) {
                return (json_object_get_int64(val));
            }
            ++i;
        }
    }
    return 0;
}

t_param *new_conf() {
    t_param *param = malloc(sizeof(*param));
    param->container_path = "~/encryptedData";
    param->container_size = MB(512);
    param->mount_point = "~/mountedContainer/";
    return param;
}

t_param *read_conf(const char *path) {
    t_param *param = new_conf();
    char *string = read_whole_file(path);

    if (string == NULL) {
        return param;
    }
    json_object *jobj = json_tokener_parse(string);
    if (json_object_get_type(jobj) != json_type_object) {
        perror("not a valid config file");
    }
    const char *container_path = find_string(jobj, "container_path");
    const char *mount_point = find_string(jobj, "mount_point");
    const size_t container_size = (const size_t) find_int(jobj, "container_size");

    param->container_path = container_path;
    param->container_size = MB(container_size);
    param->mount_point = mount_point;
    return param;
}

