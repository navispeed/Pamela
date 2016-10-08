#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "crypt.h"

void     test_urandom(){
    struct stat s = {0};

    unlink("./test");
    write_urandom("./test", 51200000);
    int rv = stat("./test", &s);
    assert(rv != -1);
    assert(s.st_size == 51200000);
    fprintf(stdout, "test_urandom PASSED\n");
}

void    test_read_conf()
{
    t_param *pParam = read_conf("./conf.json");

    assert(pParam != NULL);
    assert(pParam->container_size != 0);
    assert(pParam->container_path != NULL);
    fprintf(stdout, "test_read_conf PASSED\n");
}

void    test_volume_create()
{
  int   r;
  struct crypt_device *cd;

  // volume create (path, key, device name)
  r = volume_create("test", "1234", "device_name");
  if (r == 0)
    fprintf(stdout, "test_volume_create PASSED\n");
  else
    fprintf(stdout, "test_volume_create FAILED\n");

  // init device (path)
  cd = init_device("test");
  if (cd != NULL)
    fprintf(stdout, "test_init_device PASSED\n");
  else
    fprintf(stdout, "test_init_device FAILED\n");

  // volume mount (device name)
  r = volume_mount("device_name");
  if (r == 0)
    fprintf(stdout, "test_volume_mount PASSED\n");
  else
    fprintf(stdout, "test_volume_mount FAILED\n");

  desactivate_device("device_name");
}



int   main(int ac, char **av)
{
    test_urandom();
//    test_read_conf();
    test_volume_create();
    volume_status(av[1], av[2], av[3]);
    //volume_mount(av[3]);
    desactivate_device(av[3]);
    return (0);
}
