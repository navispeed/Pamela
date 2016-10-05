BCC=gcc

NAME=mypam.so
CFLAGS=-fPIC -fno-stack-protector

LDFLAGS=-lcryptsetup

SRC=	src/mypam.c	\
	src/crypt.c

OBJ=$(SRC:.c=.o)

TEST=	$(SRC)		\
	src/test.c

OBJ_TEST=$(TEST:.c=.o)

all:	$(NAME)

$(NAME):$(OBJ)
	$(CC) -shared -rdynamic $(LDFLAGS) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

dep:
	apt install uuid-dev libdevmapper-dev libpopt-dev libgcrypt20 libgcrypt20-dev libcryptsetup-dev

test: 	
	make -f MakefileTest re
