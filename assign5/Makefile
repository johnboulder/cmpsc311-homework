#
# CMPSC311 - Fall 2014
# Assignment #5 Makefile
#

# Variables
CC=gcc 
LINK=gcc
CFLAGS=-c -Wall -I. -fpic -g
LINKFLAGS=-L. -g
LINKLIBS=-lgcrypt 
DEPFILE=Makefile.dep

# Files to build

CRUD_CLIENT_OBJFILES=   crud_sim.o \
                        crud_file_io.o  \
                        crud_client.o \
                        crud_util.o \
                        cmpsc311_log.o \
                        cmpsc311_util.o

TARGETS=    crud_client 
                    
# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions

all : $(TARGETS) 
    
crud_client: $(CRUD_CLIENT_OBJFILES)
	$(LINK) $(LINKFLAGS) -o $@ $(CRUD_CLIENT_OBJFILES) $(LINKLIBS) 

# Do dependency generation
depend : $(DEPFILE)

$(DEPFILE) : $(CRUD_SIM_OBJFILES:.o=.c)
	gcc -MM $(CFLAGS) $(CRUD_SIM_OBJFILES:.o=.c) > $(DEPFILE)

# Cleanup 
clean:
	rm -f $(TARGETS) $(CRUD_CLIENT_OBJFILES)
  
# Dependancies
include $(DEPFILE)
