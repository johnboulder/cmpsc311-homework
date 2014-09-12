# Make environment
CC=gcc
CFLAGS=-I. -c -g -Wall
LINKARGS=-lm
LIBS=-lm

# Files
SOURCE_FILES=	cmpsc311-f14-assign2.c \
				a2support.c
OBJECT_FILES=cmpsc311-f14-assign2.o \
				a2support.o

# Productions
all : cmpsc311-f14-assign2

cmpsc311-f14-assign2 : $(OBJECT_FILES)
	$(CC) $(LINKARGS) $(OBJECT_FILES) -o $@ $(LIBS)

clean : 
	rm -f cmpsc311-f14-assign2 $(OBJECT_FILES)

