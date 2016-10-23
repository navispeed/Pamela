t_param *new_conf();
t_param *read_conf(const char *path);
int crypt_activate_device(const char *path,const char *key,const char *device_name);
int crypt_file_test(const char *path);
int write_urandom(const char *path,size_t size);
void mkfs_format(const char *path);
int crypt_volume(struct crypt_device *cd,const char *key);
void dump_device(const char __attribute__((unused))*device_name,struct crypt_device __attribute__((unused))*cd);
int volume_format(struct crypt_device *cd,struct crypt_params_luks1 params,const char *key,const char *device_name,const char *path);
struct crypt_device *init_device(const char *path);
int volume_create(const char *path,const char *key,const char *device_name);
int desactivate_device(const char *device_name);
int volume_mount(const char *device_name,const char *dest);
int volume_umount(const char *device_name);
int volume_status(const char __attribute__((unused))*path,const char __attribute__((unused))*key,const char *device_name);
const char *get_user_home(const char *username);
const char *get_real_path(const char *path,const char *username);
void test_urandom();
void test_read_conf();
void test_volume_create();
int test_get_user_home();
int test_replace_by_home();
int main(int ac,char **av);
int main(int argc,char *argv[]);
int main(int argc,char **argv);
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh,int __attribute__((unused))flags,int __attribute__((unused))argc,const char __attribute__((unused))**argv);
int pam_sm_open_session(pam_handle_t *pamh,int __attribute__((unused))flags,int __attribute__((unused))argc,const char __attribute__((unused))**argv);
int pam_sm_close_session(pam_handle_t __attribute__((unused))*pamh,int __attribute__((unused))flags,int __attribute__((unused))argc,const char __attribute__((unused))**argv);
const char *getFinalPass(pam_handle_t *pamh,const char **pass);
char *strdup(const char *);
int call_shell_script(struct pam_handle *pamh,const char **key);
extern const struct pam_conv conv;
void pam_usb_init();
uint16_t find_sn_from_filename(const char *filename);
void show_all_usb();
#define INTERFACE 0
#define EXPORT_INTERFACE 0
#define LOCAL_INTERFACE 0
#define EXPORT
#define LOCAL static
#define PUBLIC
#define PRIVATE
#define PROTECTED
