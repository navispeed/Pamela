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
        {NULL, 0}
};

static char	*read_whole_file(const char *path)
{
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

t_param		*read_conf(const char *path)
{
  t_param	*param = malloc(sizeof(*param));
  char		*string = read_whole_file(path);

  json_object * jobj = json_tokener_parse(string);
  if (json_object_get_type(jobj) != json_type_object)
    {
      perror("not a valid config file");
    }
  enum json_type type;
  json_object_object_foreach(jobj, key, val) {
    int i = 0;

    while (strToMember[i].name) {
      if (strcmp(key, strToMember[i].name)) {
        void *dest = (param + strToMember[i].shift);

      }
      ++i;
    }
  }
  return param;
}

