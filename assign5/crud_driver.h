#ifndef CRUD_DRIVER_INCLUDED
#define CRUD_DRIVER_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_driver.h
//  Description    : This is the header file for the driver implementation
//                   of the CRUD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Sat Sep  6 08:24:25 EDT 2014
//

// Includes
#include <stdint.h>

// Defines
#define CRUD_MAX_OBJECT_SIZE 0xfffff
#define CRUD_NO_OBJECT 0

//
// Type definitions

typedef uint32_t CrudOID; // This is the request object identifier (unique to object)

// These are the request types
typedef enum {
	CRUD_INIT    = 0, // Initialize the CRUD interface
	CRUD_FORMAT  = 1, // Format the CRUD storage device
	CRUD_CREATE  = 2, // Create a new object
	CRUD_READ    = 3, // Read an object
	CRUD_UPDATE  = 4, // Update the object
	CRUD_DELETE  = 5, // Delete an object
	CRUD_CLOSE   = 6, // Close the CRUD device
	CRUD_UNKNOWN = 7, // Unknown type
	CRUD_MAXVAL  = 8, // Max value
} CRUD_REQUEST_TYPES;
const char *CRUD_REQUEST_TYPE_LABLES[CRUD_MAXVAL];

// These are the CRUD flags
typedef enum {
	CRUD_NULL_FLAG       = 0,  // This is the "no flag" flag
	CRUD_PRIORITY_OBJECT = 1,  // Flag indicating that object is a "priority object"
	CRUD_FLAGMAX         = 2,  // Max value
} CRUD_FLAG_TYPES;
const char *CRUD_FLAG_TYPE_LABLES[CRUD_FLAGMAX];

// CRUD request and response types
typedef uint64_t CrudRequest;
typedef uint64_t CrudResponse;

/*

 Request/Response Specification

  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                               OID                             |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |  Req  |                   Length                      |Flags|R|
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  Bits    Description
  -----   -------------------------------------------------------------
   0-31 - OID - the object ID (0 if not relevant)
  32-35 - Request type - this is the request type (CRUD_REQUEST_TYPES)
  36-59 - Length - this is the size of the object in bytes
  60-62 - Flags - these are flags for commands (UNUSED)
     63 - R - this is the result bit (0 success, 1 is failure)

*/

//
// CRUD interface

CrudResponse crud_bus_request( CrudRequest request, void *buf );
	// This is the interface to the CRUD interfaces

int crud_save_store(char *fname);
	// Write the contents of the CRUD store to disk file.

int crud_load_store(char *fname);
	// Read the contents of the storage device from a disk file.

//
// Unit testing for the module

int crud_unit_test( void );
	// This is a function used to test the CRUD interfaces and code.

//
// Utility functions (see crud_util.c)

CrudRequest construct_crud_request(CrudOID oid, CRUD_REQUEST_TYPES req,
		uint32_t length, uint8_t flags, uint8_t res);
    // Create a 64-bit bus request buffer

int deconstruct_crud_request(CrudRequest request, CrudOID *oid,
		CRUD_REQUEST_TYPES *req, uint32_t *length, uint8_t *flags,
		uint8_t *res);
    // Extract values from a 64-bit bus request buffer

#endif
