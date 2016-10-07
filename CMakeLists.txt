cmake_minimum_required (VERSION 2.6)
project (Tutorial)
include_directories(include)

set(CFLAGS -std=gnu11)

add_executable(mypam src/crypt.c src/main_test.c src/conf.c src/pam.c include/crypt.h src/files.c)
target_link_libraries(mypam -lcryptsetup json -lpam -lpam_misc)
