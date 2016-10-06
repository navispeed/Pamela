BCC=gcc

NAME=mypam.so
CFLAGS=-fPIC -fno-stack-protector -I include -std=c11 -W -Wall -Wextra

LDFLAGS=-lcryptsetup -l json

SRC=	src/pam.c	\
	src/crypt.c	\
	src/conf.c	

OBJ=$(SRC:.c=.o)

TEST=	$(SRC)		\
	src/test.c

all:	$(NAME)

$(NAME):$(OBJ)
	$(CC) -shared -rdynamic $(LDFLAGS) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

dep:
	apt install uuid-dev libdevmapper-dev libpopt-dev libgcrypt20 libgcrypt20-dev libcryptsetup-dev libjson0 libjson0-dev libpam0g-dev

test:
	make -f MakefileTest re

install:all
	sudo ld -x --shared -o /lib/security/mypam.so mypam.o
