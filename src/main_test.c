#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "crypt.h"
#include <string.h>

void test_urandom() {
    struct stat s = {0};

    unlink("./test");
    write_urandom("./test", 51200000);
    int rv = stat("./test", &s);
    assert(rv != -1);
    assert(s.st_size == 51200000);
    fprintf(stdout, "test_urandom PASSED\n");
}

void test_read_conf() {
    t_param *pParam = read_conf("./conf.json");

    assert(pParam != NULL);
    assert(pParam->container_size != 0);
    assert(pParam->container_path != NULL);
    fprintf(stdout, "test_read_conf PASSED\n");
}

void test_volume_create() {
    int r;
    struct crypt_device *cd;

    // volume create (path, key, device name)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[34;01mtest_volume_create\033[0m\n");
    r = volume_create("test", "1234", "device_name");
    if (r >= 0)
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[32;01mtest_volume_create PASSED\033[0m\n\n");
    else
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[31;01mtest_volume_create FAILED\033[0m\n\n");

    assert(r >= 0);

    // init device (path)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[34;01mtest_init_device\033[0m\n");
    cd = init_device("test");
    if (cd != NULL)
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[32;01mtest_init_device PASSED\033[0m\n\n");
    else
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[31;01mtest_init_device FAILED\033[0m\n\n");

    assert(r >= 0);

    // volume mount (device name)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[34;01mtest_volume_mount\033[0m\n");
    r = volume_mount("device_name", "/mnt");
    if (r == 0)
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[32;01mtest_volume_mount PASSED\033[0m\n\n");
    else
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[31;01mtest_volume_mount FAILED\033[0m\n\n");

    assert(r >= 0);

    volume_umount("device_name");
    desactivate_device("device_name");

    // crypt file test (path)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[34;01mtest_crypt_file_test\033[0m\n");
    r = crypt_file_test("test");
    if (r == 0)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[32;01mtest_crypt_file_test PASSED\033[0m\n\n");
    else
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[31;01mtest_crypt_file_test FAILED\033[0m\n\n");

    //crypt activate device (path, key, device_name)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[34;01mtest_crypt_activate_device\033[0m\n");
    r = crypt_activate_device("test", "1234", "device_name");
    if (r == 0)
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[32;01mtest_crypt_activate_device PASSED\033[0m\n\n");
    else
    fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~\033[31;01mtest_crypt_activate_device FAILED\033[0m\n\n");

    desactivate_device("device_name");
}

int test_get_user_home()
{
    unsetenv("HOME");
    assert(strcmp("/root", get_user_home("root")) == 0);
    assert(strcmp("/home/greg", get_user_home("greg")) == 0);
    fprintf(stdout, "test_get_user_home PASSED\n");
    return 0;
}

int test_replace_by_home()
{
    assert(strcmp("/root/conf.json", get_real_path("~/conf.json", "root")) == 0);
    assert(strcmp("/home/greg/conf.json", get_real_path("~/conf.json", "greg")) == 0);
}


int main(int ac, char **av) {
    test_urandom();
    test_get_user_home();
    test_replace_by_home();
    test_read_conf();
    test_volume_create();
    return (0);
}
