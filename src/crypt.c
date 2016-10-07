#include <stdbool.h>
#include <stdio.h>
#include <libcryptsetup.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "crypt.h"

#define BS 4096

/**
 * Destroy specified file
 * @param path to file
 * @param size (in bytes)
 * @return
 */
int	        write_urandom(const char *path, size_t size)
{
  int		fd_in;
  int		fd_out;
  size_t	readed = 0;
  char  	buf[BS];

  fd_in = open("/dev/urandom", O_RDONLY);
  fd_out = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd_in == -1 || fd_out == -1)
    {
      fprintf(stderr, "cannot open %s file", (fd_in == -1 ? "input" : "output"));
      return (-1);
    }
  while (size > 0 && (readed = (size_t) read(fd_in, buf, BS)) > 0)
    {
      write(fd_out, buf, readed);
      size = (size > readed ? size - readed : readed - size);
    }
  return (0);
}

static void     mkfs_format(const char *path)
{
    char        buff[BS];
    snprintf(buff, BS, "mkfs.ext4 %s", path);
    system(buff);
}

static int       volume_format(struct crypt_device *cd, struct crypt_params_luks1 params)
{
  if (crypt_format(cd, CRYPT_LUKS1, "aes", "xts-plain64", NULL, NULL, 256 / 8,
      &params) < 0)
    {
      fprintf(stderr, "crypt_format() failed\n");
      perror("FORMAT");
      return (-1);
    }
  return (0);
}

int	                          volume_create(const char *path, const char *key)
{
  struct crypt_device         *cd;
  struct crypt_params_luks1   params;
  int                         r;

  if (geteuid())
    {
      printf("Using of libcryptsetup requires super user privileges.\n");
      return (1);
    }
  if ((r = crypt_init(&cd, path)) < 0) //TODO : migrate to function init()
    {
      fprintf(stderr, "crypt_init failed on path %s with error %d\n", path, r);
      perror("INIT");
      return (-1);
    }
  printf("Context is attached to the block %s", crypt_get_device_name(cd));
  params.hash = "sha1";
  params.data_alignment = 0;
  params.data_device = NULL;
  volume_format(cd, params);
  return (0);
}

