t_param *new_conf();
t_param *read_conf(const char *path);
int write_urandom(const char *path,size_t size);
int crypt_volume(struct crypt_device *cd,const char *key);
int format(struct crypt_device *cd,struct crypt_params_luks1 params,const char *key,const char *device_name);
struct crypt_device *init_device(struct crypt_device *cd,const char *path);
int volume_create(const char *path,const char *key,const char *device_name);
void test_urandom();
void test_read_conf();
void test_volume_create();
int main(int ac,char **av);
int main(int argc,char *argv[]);
int main(int argc,char **argv);
PAM_EXTERN int pam_sm_authenticate(__attribute__((unused))pam_handle_t *pamh,__attribute__((unused))int flags,__attribute__((unused))int argc,__attribute__((unused))const char **argv);
extern const struct pam_conv conv;
#define INTERFACE 0
#define EXPORT_INTERFACE 0
#define LOCAL_INTERFACE 0
#define EXPORT
#define LOCAL static
#define PUBLIC
#define PRIVATE
#define PROTECTED
