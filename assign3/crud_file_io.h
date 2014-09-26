#ifndef CRUD_FILE_IO_INCLUDED
#define CRUD_FILE_IO_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_file_io.h
//  Description    : This is the header file for the standardized IO functions
//                   for used to access the CRUD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Tue Sep 16 19:38:42 EDT 2014
//

// Include files
#include <stdint.h>

//
// Interface functions

int16_t crud_open(char *path);
	// This function opens the file and returns a file handle

int16_t crud_close(int16_t fd);
	// This function closes the file

int32_t crud_read(int16_t fd, void *buf, int32_t count);
	// Reads "count" bytes from the file handle "fh" into the buffer  "buf"

int32_t crud_write(int16_t fd, void *buf, int32_t count);
	// Writes "count" bytes to the file handle "fh" from the buffer  "buf"

int32_t crud_seek(int16_t fd, uint32_t loc);
	// Seek to specific point in the file

//
// Unit testing for the module

int crudIOUnitTest(void);
	// Perform a test of the CRUD IO implementation

#endif


