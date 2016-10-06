cmake_minimum_required (VERSION 2.6)
project (Tutorial)
include_directories(include)



add_executable(mypam src/crypt.c src/main_test.c src/conf.c src/pam.c include/crypt.h)
target_link_libraries(mypam -lcryptsetup json -lpam -lpam_misc)
