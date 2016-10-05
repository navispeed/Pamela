#include <stdbool.h>
#include <stdio.h>
#include <libcryptsetup.h>

int       init(struct crypt_device *cd, const char *path)
{
  if (crypt_init(&cd, path) < 0);
  {
    fprintf(stderr, "crypt_init failed on path %s\n", path);
    perror("INIT");
    return (-1);
  }
  return (0);
}

int       format(struct crypt_device *cd, struct crypt_params_luks1 params)
{
  if (crypt_format(cd, CRYPT_LUKS1, "aes", "xts-plain64", NULL, NULL, 256 / 8,
      &params) < 0)
    {
      fprintf(stderr, "crypt_format() failed\n");
      return (-1);
    }
  return (0);
}

int	                          volume_create(const char *path, const char *key)
{
  struct crypt_device         *cd;
  struct crypt_params_luks1   params;

  if (init(cd, path) == 0)
    {
      printf("Context is attached to the block %s", crypt_get_device_name(cd));
      params.hash = "sha1";
      params.data_alignment = 0;
      params.data_device = NULL;
      format(cd, params);
    }
  fprintf(stderr, "Runtime error\n");
  return (-1);
}
