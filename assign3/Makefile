#
# CMPSC311 - Fall 2014
# Assignment #3 Makeeil
#
#

# Variables
ARCHIVE=ar
CC=gcc 
LINK=gcc
CFLAGS=-c -Wall -I. -fpic -g
LINKFLAGS=-L. -g
LIBFLAGS=-shared -Wall
LINKLIBS=-lcrud -lgcrypt 
DEPFILE=Makefile.dep

# Files to build

CRUD_SIM_OBJFILES=	crud_sim.o \
					crud_file_io.o

TARGETS=	crud_sim 
					
# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions

all : $(TARGETS) 
	
crud_sim : $(CRUD_SIM_OBJFILES) libcrud.a
	$(LINK) $(LINKFLAGS) -o $@ $(CRUD_SIM_OBJFILES) $(LINKLIBS) 

# Do dependency generation
depend : $(DEPFILE)

$(DEPFILE) : $(CRUD_SIM_OBJFILES:.o=.c)
	gcc -MM $(CFLAGS) $(CRUD_SIM_OBJFILES:.o=.c) > $(DEPFILE)
        
# Cleanup 
clean:
	rm -f $(TARGETS) $(CRUD_SIM_OBJFILES) 
  
# Dependancies
