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

// Project include files
#include <crud_driver.h>

// Defines
#define CRUD_MAX_TOTAL_FILES 1024
#define CRUD_MAX_PATH_LENGTH 128

// Type definitions

// This is the basic file handle structure (note: index into file table is fh)
typedef struct {
	char      filename[CRUD_MAX_PATH_LENGTH]; // The filename of the data to be manipulated
	CrudOID   object_id;                      // The handle of the object
	uint32_t  position;                       // This is the position of the file
	uint32_t  length;                         // This is the length of the file
	uint8_t   open;                           // Flag indicating the file is currently open
} CrudFileAllocationType;

//
// Management operations

uint16_t crud_format(void);
	// This function formats the CRUD drive, and adds the file allocation table.

uint16_t crud_mount(void);
	// This function mount the current crud file system and loads the file allocation table.

uint16_t crud_unmount(void);
	// This function unmounts the current crud file system and saves the file allocation table.

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


