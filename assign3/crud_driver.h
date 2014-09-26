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
	CRUD_CREATE  = 1, // Create a new object
	CRUD_READ    = 2, // Read an object
	CRUD_UPDATE  = 3, // Update the object
	CRUD_DELETE  = 4, // Delete an object
	CRUD_UNKNOWN = 5, // Unknown type
	CRUD_MAXVAL  = 6, // Max value
} CRUD_REQUEST_TYPES;

const char *CRUD_REQUEST_TYPE_LABLES[CRUD_MAXVAL];

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
  36-58 - Length - this is the size of the object in bytes
  60-62 - Flags - these are flags for commands (UNUSED)
     63 - R - this is the result bit (0 success, 1 is failure)

*/

//
// CRUD interface

CrudResponse crud_bus_request( CrudRequest request, void *buf );
	// This is the interface to the CRUD interfaces

//
// Unit testing for the module

int crud_unit_test( void );
	// This is a function used to test the CRUD interfaces and code.

#endif
