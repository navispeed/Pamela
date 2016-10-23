CC=gcc

NAME=mypam.so
CFLAGS=-fPIC -fno-stack-protector -I include -std=c11 -W -Wall -Wextra

LDFLAGS=-lcryptsetup -l json -lusb

SRC=	src/pam.c	\
	src/crypt.c	\
	src/conf.c	\
	src/files.c	\
	src/usb.c   \
	src/shell.c \


OBJ=$(SRC:.c=.o)

all:	$(NAME)

$(NAME):$(OBJ)
	$(CC) $(OBJ) -shared -rdynamic $(LDFLAGS) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

dep:
	apt install uuid-dev libdevmapper-dev libpopt-dev libgcrypt20 libgcrypt20-dev libcryptsetup-dev libjson0 libjson0-dev libpam0g-dev libusb-dev -y

test:
	make -f MakefileTest re

install:all
	sudo ld -x --shared -o /lib/security/mypam.so mypam.o
