#include <stdbool.h>
#include <stdio.h>
#include <libcryptsetup.h>
#include <string.h>

int                           free_crypt(struct crypt_device *cd)
{
  crypt_free(cd);
  return (0);
}

int                           crypt_volume(struct crypt_device *cd,
                                           const char *key)
{
  if (crypt_keyslot_add_by_volume_key(cd, CRYPT_ANY_SLOT,
                                      NULL, 0, key, strlen(key)) < 0)
     {
       fprintf(stderr, "crypt_keyslot_add_by_volume_key() failed\n");
       perror("KEYSLOT ADD BY VOLUME KEY");
       return (-1);
     }
  return (0);
}

int                           format(struct crypt_device *cd,
                                     struct crypt_params_luks1 params,
                                     const char *key,
                                     const char *device_name)
{
  struct crypt_active_device  cad;

  if (crypt_format(cd, CRYPT_LUKS1, "aes", "xts-plain64", NULL, NULL, 256 / 8,
      &params) < 0)
    {
      fprintf(stderr, "crypt_format() failed\n");
      perror("FORMAT");
      return (-1);
    }
  crypt_load(cd, CRYPT_LUKS1, NULL);
  crypt_activate_by_passphrase(cd, device_name, CRYPT_ANY_SLOT,
                               key, strlen(key), CRYPT_ACTIVATE_READONLY);
  crypt_get_active_device(cd, device_name, &cad);
  return (0);
}

int	                          volume_create(const char *path, const char *key,
                                            const char *device_name)
{
  struct crypt_device         *cd;
  struct crypt_params_luks1   params;
  int                         r;

  if (geteuid())
    {
      printf("Using of libcryptsetup requires super user privileges.\n");
      return (1);
    }
  if ((r = crypt_init(&cd, path)) < 0)
    {
      fprintf(stderr, "crypt_init failed on path %s with error %d\n", path, r);
      perror("INIT");
      return (-1);
    }
  printf("Context is attached to the block %s", crypt_get_device_name(cd));
  params.hash = "sha1";
  params.data_alignment = 0;
  params.data_device = NULL;
  if (format(cd, params, key, device_name) < 0)
    fprintf(stderr, "crypt_init failed on path %s with error %d\n", path, r);
  crypt_free(cd);
  return (-1);
}
