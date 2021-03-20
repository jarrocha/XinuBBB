#
# Make the Xinu configuration program
#

COMPILER_ROOT	=	/usr/bin/

CC	= ${COMPILER_ROOT}gcc
LEX	= ${COMPILER_ROOT}flex
YACC	= ${COMPILER_ROOT}bison -y	# Flag enables yacc-compatible filenames
CFLAGS	= 
LFLAGS	= -lfl

#
# Name of the configuration program
#

CONFIG	= config

all:		conf.h conf.c

${CONFIG}:	lex.yy.c y.tab.c
		$(CC) ${CFLAGS} -o $@ y.tab.c ${LFLAGS}

lex.yy.c:	config.l
		$(LEX) config.l

y.tab.c:	config.y
		$(YACC) config.y

clean:
		rm -f config lex.yy.c y.tab.c

conf.h:		config Configuration
		./config Configuration conf.c conf.h

conf.c:		config Configuration
		./config Configuration conf.c conf.h

install:	conf.h conf.c
		cp -p conf.h ../include
		cp -p conf.c ../system
