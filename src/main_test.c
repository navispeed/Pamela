#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "crypt.h"

void     test_urandom(){
    struct stat s = {0};

    unlink("./test");
    write_urandom("./test", 512000);
    int rv = stat("./test", &s);
    assert(rv != -1);
    assert(s.st_size == 512000);
    fprintf(stdout, "test_urandom PASSED");
}

void    test_read_conf()
{
    t_param *pParam = read_conf("./conf.json");

    assert(pParam != NULL);
}

void test_volume_create()
{

}

int   main(int ac, char **av)
{
    test_urandom();
    test_read_conf();
  //    volume_create(av[1], av[2]);
    return (0);
}
