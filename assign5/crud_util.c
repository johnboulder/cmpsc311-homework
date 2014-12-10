////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_util.c
//  Description    : This is the implementation of the driver utility functions
//                   for the CRUD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Wed Nov  5 08:22:39 PST 2014
//

// Includes

// Project includes
#include <crud_driver.h>

// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : construct_crud_request
// Description  : Construct the CRUD command from the request fields
//
// Inputs       : request - the request structure (64 bits, see crud_driver.h)
//                oid - the object ID
//                req - the request type
//                length - the size of the object in bytes
//                flags - the flags associated with the response
//                res - the result flag
// Outputs      : 0 if successful test, -1 if failure

CrudRequest construct_crud_request(CrudOID oid, CRUD_REQUEST_TYPES req,
		uint32_t length, uint8_t flags, uint8_t res) {

	// Build up the request fields
	CrudRequest request = 0;
	request = ((uint64_t) oid) << 32;
	request |= req << 28;
	request |= length << 4;
	request |= flags << 1;
	request |= res;

	// Return successfully
	return (request);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : deconstruct_crud_request
// Description  : Extract the CRUD command fields from the request byte stream
//
// Inputs       : request - the request structure (64 bits, see crud_driver.h)
//                oid - the place to put the object ID
//                req - the place to put the request type
//                length - the size of the object in bytes
//                flags - the flags associated
//                res - the result flag
// Outputs      : 0 if successful test, -1 if failure

int deconstruct_crud_request(CrudRequest request, CrudOID *oid,
		CRUD_REQUEST_TYPES *req, uint32_t *length, uint8_t *flags,
		uint8_t *res) {

	// Pull out the fields
	uint32_t bottom;
	*oid = request >> 32;
	bottom = (uint32_t) request & 0xffffffff;
	*req = bottom >> 28;
	*length = (bottom << 4) >> 8;
	*flags = (bottom & 0xe) >> 1;
	*res = (bottom & 1);

	// Return successfully
	return (0);
}
