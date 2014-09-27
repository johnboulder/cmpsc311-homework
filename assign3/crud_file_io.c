////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_file_io.h
//  Description    : This is the implementation of the standardized IO functions
//                   for used to access the CRUD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Tue Sep 16 19:38:42 EDT 2014
//
/* This file is the only thing we have to change for this assignment
 * 
 * TODO 
 * Everytime someone calls something like open or read, we're going to perform
 * that shit here.*/
// 
// There are examples in the slides of evrything we need to do. THey are currently wrong. Email him about it
// slides will be fixed by tomorrow
//
// CRUD - stands for Create, read, update, and delete - 4 options we have
// Objects are immutable. This means that we cannot resize the data we allocated to the object. we have to delete it and then rewrite it somewhere else 
//
// This code is some of the most important stuff in an OS
//
// crud_init- initialization function. This must be called before we do anything. We need to check if we've initialized the crud interface before
// we call crud_init. If we haven't, then it's the first thing we should do.
//
// crud_create- block of data and length and a command, and it will create an object in the object store.
//
// when we open a file. we do init, and then we have to create our fileHandle, and ensure that it's not already initialized
// gotta make sure stuff like buffers aren't null
//
// open: assign fd file len=0
// read: return because file len is 0. pos = 0;
// write: create object of specified length using crud_init. objectio = x. file length = 7. pos = 7
// 
// crud_init returns an object id
// 
// all files share the same bitposition
//
// CANNOT HOLD THE DATA OF THE FILE LOCALLY. ONCE YOU RETURN FROM ANY OF THE FUNCTIONS. YOU CANNOT HOLD ANYTHING 
// BUT THE FILE DESCRIPTOR, THE POSITION, AND THE OBJECT ID. ONLY THINGS YOU CAN HOLD ON TO IN YOUR DRIVER
// WANT TO KEEP AMOUNT OF INFO IN DRIVER AS SMALL AS POSSIBLE
// can just make 3 global variables for these three things
//
// assume:
// fd=16
// oid=906
// dos = 0
// we do a read 3:
// check for good values
// get(906) which pulls data from the object store into our program. it always pulls that entire object. we never pull partial objects
// those 3 bytes are read into a buffer, and then returned.
// 
// filedescriptor can be a constant int that we assign for this assingment. we'll never be dealing with multiple objects at once.
//
// WE GET PERFORMANCE IMPROVEMENT FROM NEVER APPENDING TO AN EXISTING OBJECT. WE ALWAYS CREATE A NEW OBJECT, DELETE THE OLD ONE, AND THEN WRITE THE 
// OBJECT TO SOME PLACE.
//
// the data we are dealing with is untyped data as far as the buffers go.
//
// CRUD_MAX is the max size any file can be for this assignment
//
// anticipate for the next assignment.
//
// PREPARE FOR MAINTAINING MORE THAN JUST ONE FILE REFERENCE
//
// can perform an update instead of a delete/create if you're writing to the middle of a file
//
// crud_update - replaces the data in an object. must be the exact same size.
//
// how we interact with hardware
//
// when we interact with the crud interface, the only thing we call is called the crud bus
// 
// Big endian?
// 
// may not ever use flags in this class on the crud bus
//
// responses come back in the same exact format, but with all the information referring to the properties of the object
//
// when we run the unit test
// ./crud_sim -u -v
// CRUD unit test completed successfully
//
// LOOK INTO THE UNIT TEST BEFORE WE GET STARTED. AND FIGURE OUT WHAT IT'S DOING. IT WILL GIVE INSIGHT INTO YOUR CODE
//
// cmpsc311.log.h, there is a log message that you should look at for debugging
// make log output that sends itself to the log and you know what's going on
// THE LOG IS COMING OUT OF STDERR
//
// the 64 bits:
// first 32 - 0 to 31 are the OID
//
// uint64_t CrudRequest create_crudrequest(uint32-t oid, req, length, flags, R)
// v = oid<<32
// v = req<<28 : oid is now gone.
// therefore, we want to do v = v|(req<<28) LOOK AT THE GRAPHIC FOR A BETTER UNDERSTANDING
// 
// first time we get a read, we create an object in the object store
// Includes
#include <malloc.h>
#include <string.h>

// Project Includes
#include <crud_file_io.h>
#include <crud_driver.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

/*Basic summary of bullshit:
 * -Interaction with crud interface is based on methods defined in crud_driver.h
 * -This interface contains a single function call with two arguments
 *  64-bit CRUD bus request value (type CrudRequest)
 *  a pointer to a variable-sized buffer
 *  CrudResponse crud_bus_request(CrudRequest request, void *buf);
 *  */


// Defines 
#define CIO_UNIT_TEST_MAX_WRITE_SIZE 1024
#define CRUD_IO_UNIT_TEST_ITERATIONS 10240
#define FILE_HANDLE_BASE_VALUE 16

int8_t crud_initialized = 0;
int16_t curret_handle = FILE_HANDLE_BASE_VALUE;
uint32_t current_position = 0;
// File structure.
// May in future become useful in keeping track of a shitload of concurrent files
struct file_st 
{
	//TODO find out if we have to remove any of this stuff to comply with what we're keeping track of
	int32_t oid;
	int8_t request;
	int32_t length;
	int8_t flags;
	int8_t result;
	int16_t file_handle; // Known as fd in most functions
};

// Type for UNIT test interface
typedef enum 
{
    CIO_UNIT_TEST_READ   = 0,
    CIO_UNIT_TEST_WRITE  = 1,
    CIO_UNIT_TEST_APPEND = 2,
    CIO_UNIT_TEST_SEEK   = 3,
} CRUD_UNIT_TEST_TYPE;

//
// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_open
// Description  : This function opens the file and returns a file handle
//
// Inputs       : path - the path "in the storage array"
// Outputs      : file handle if successful, -1 if failure
// make an association between path and a file handle
// we'll return an integer that is our file handle
// once returned, all context shit relating to the position in the file can disappear. this is done by close
//
int16_t crud_open(char *path) 
{
	if(!crud_initialized)
		crud_init();
	struct file_st responseFile = processResponse();
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_close
// Description  : This function closes the file
//
// Inputs       : fd - the file handle of the object to close
// Outputs      : 0 if successful, -1 if failure

int16_t crud_close(int16_t fh) 
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_read
// Description  : Reads up to "count" bytes from the file handle "fh" into the
//                buffer  "buf".
//
// Inputs       : fd - the file descriptor for the read
//                buf - the buffer to place the bytes into
//                count - the number of bytes to read
// Outputs      : the number of bytes read or -1 if failures

int32_t crud_read(int16_t fd, void *buf, int32_t count) 
{
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_write
// Description  : Writes "count" bytes to the file handle "fd" from the
//                buffer  "buf"
//
// Inputs       : fd - the file descriptor for the file to write to
//                buf - the buffer to write
//                count - the number of bytes to write
// Outputs      : the number of bytes written or -1 if failure
//
// If we get an error from the hardware, we did something wrong.
//
int32_t crud_write(int16_t fd, void *buf, int32_t count) 
{
// Add some number of bytes to the front of the file 
// If count is past current end of file, write over the end of the file
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_seek
// Description  : Seek to specific point in the file
//
// Inputs       : fd - the file descriptor for the file to seek
//                loc - offset from beginning of file to seek to
// Outputs      : 0 if successful or -1 if failure

int32_t crud_seek(int16_t fd, uint32_t loc) 
{
// You'll never get a seek that's outside the scope of your program in this assignment (assign3)

}

int16_t crud_init()
{
	void *buf;
	CrudRequest req = createRequest(0, CRUD_INIT, 0);
	CrudResponse res = crud_bus_request(req, buf);
	processResponse(res);
}

CrudRequest createRequest(int32_t oid, int8_t request, int32_t length)
{
	CrudRequest req = 0;
	
	// Add the 32 bits of oid to the 64 least significant bits of req
	req+= oid;
	req<<4;

	req+= (15 & request);
	req<<24;

	req+= (16777215 & length);
	req<<3;

	//TODO confirm that we don't actually need to use flags or result bits in any requests
	req<<1;

	return req;
}

file_st processResponse(CrudResponse res)
{
	/* TODO
	 * find out how to correctly create logs, and log an error for when the result code in response is 1 (failure)
	 */
	int i = 0;
	struct file_st file;
	file.result = (int8_t)( 1 & res );
	// TODO include check here for response code of 1
	res>>=1;
	
	// Flags
	file.flags = (int8_t)( 7 & res );
	res>>=3;

	// Length
	file.length = (int32_t)(16777215 & res);
	res>>=24;

	// Request
	file.request = (int8_t)(15 & res);
	res>>=4;

	// OID
	file.request = (int32_t)(4294967295 & res);
	// No need to shift res again.
	
	// Get the file handle
	file.file_handle = getNewHandle();
}

int16_t getNewHandle()
{
	return current_handle+=1;
}

// Sit down, and look through interface to see how reads, writes and whatever else does what it does.
// Unit Test Function

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crudIOUnitTest
// Description  : Perform a test of the CRUD IO implementation
//
// Inputs       : None
// Outputs      : 0 if successful or -1 if failure

int crudIOUnitTest(void) {

	// Local variables
	uint8_t ch;
	int16_t fh, i;
	int32_t cio_utest_length, cio_utest_position, count, bytes, expected;
	char *cio_utest_buffer, *tbuf;
	CRUD_UNIT_TEST_TYPE cmd;
	char lstr[1024];

	// Setup some operating buffers, zero out the mirrored file contents
	cio_utest_buffer = malloc(CRUD_MAX_OBJECT_SIZE);
	tbuf = malloc(CRUD_MAX_OBJECT_SIZE);
	memset(cio_utest_buffer, 0x0, CRUD_MAX_OBJECT_SIZE);
	cio_utest_length = 0;
	cio_utest_position = 0;

	// Start by opening a file
	fh = crud_open("temp_file.txt");
	if (fh == -1) {
		logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Failure open operation.");
		return(-1);
	}

	// Now do a bunch of operations
	for (i=0; i<CRUD_IO_UNIT_TEST_ITERATIONS; i++) {

		// Pick a random command
		if (cio_utest_length == 0) {
			cmd = CIO_UNIT_TEST_WRITE;
		} else {
			cmd = getRandomValue(CIO_UNIT_TEST_READ, CIO_UNIT_TEST_SEEK);
		}

		// Execute the command
		switch (cmd) {

		case CIO_UNIT_TEST_READ: // read a random set of data
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : read %d at position %d", bytes, cio_utest_position);
			bytes = crud_read(fh, tbuf, count);
			if (bytes == -1) {
				logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Read failure.");
				return(-1);
			}

			// Compare to what we expected
			if (cio_utest_position+count > cio_utest_length) {
				expected = cio_utest_length-cio_utest_position;
			} else {
				expected = count;
			}
			if (bytes != expected) {
				logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : short/long read of [%d!=%d]", bytes, expected);
				return(-1);
			}
			if ( (bytes > 0) && (memcmp(&cio_utest_buffer[cio_utest_position], tbuf, bytes)) ) {

				bufToString((unsigned char *)tbuf, bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST R: %s", lstr);
				bufToString((unsigned char *)&cio_utest_buffer[cio_utest_position], bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST U: %s", lstr);

				logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : read data mismatch (%d)", bytes);
				return(-1);
			}
			logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : read %d match", bytes);


			// update the position pointer
			cio_utest_position += bytes;
			break;

		case CIO_UNIT_TEST_APPEND: // Append data onto the end of the file
			// Create random block, check to make sure that the write is not too large
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			if (cio_utest_length+count >= CRUD_MAX_OBJECT_SIZE) {

				// Log, seek to end of file, create random value
				logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : append of %d bytes [%x]", count, ch);
				logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : seek to position %d", cio_utest_length);
				if (crud_seek(fh, cio_utest_length)) {
					logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : seek failed [%d].", cio_utest_length);
					return(-1);
				}
				cio_utest_position = cio_utest_length;
				memset(&cio_utest_buffer[cio_utest_position], ch, count);

				// Now write
				bytes = crud_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes != count) {
					logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : append failed [%d].", count);
					return(-1);
				}
				cio_utest_length = cio_utest_position += bytes;
			}
			break;

		case CIO_UNIT_TEST_WRITE: // Write random block to the file
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			// Check to make sure that the write is not too large
			if (cio_utest_length+count < CRUD_MAX_OBJECT_SIZE) {
				// Log the write, perform it
				logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : write of %d bytes [%x]", count, ch);
				memset(&cio_utest_buffer[cio_utest_position], ch, count);
				bytes = crud_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes!=count) {
					logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : write failed [%d].", count);
					return(-1);
				}
				cio_utest_position += bytes;
				if (cio_utest_position > cio_utest_length) {
					cio_utest_length = cio_utest_position;
				}
			}
			break;

		case CIO_UNIT_TEST_SEEK:
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : seek to position %d", count);
			if (crud_seek(fh, count)) {
				logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : seek failed [%d].", count);
				return(-1);
			}
			cio_utest_position = count;
			break;

		default: // This should never happen
			CMPSC_ASSERT0(0, "CRUD_IO_UNIT_TEST : illegal test command.");
			break;

		}

#if DEEP_DEBUG
		// VALIDATION STEP: ENSURE OUR LOCAL IS LIKE OBJECT STORE
		CrudRequest request;
		CrudResponse response;
		CrudOID oid;
		CRUD_REQUEST_TYPES req;
		uint32_t length;
		uint8_t res, flags;

		// Make a fake request to get file handle, then check it
		request = construct_crud_request(file_table[0].object_handle, CRUD_READ, CRUD_MAX_OBJECT_SIZE, 0, 0);
		response = crud_bus_request(request, tbuf);
		if ((deconstruct_crud_request(response, &oid, &req, &length, &flags, &res) != 0) || (res != 0))  {
			logMessage(LOG_ERROR_LEVEL, "Read failure, bad CRUD response [%x]", response);
			return(-1);
		}
		if ( (cio_utest_length != length) || (memcmp(cio_utest_buffer, tbuf, length)) ) {
			logMessage(LOG_ERROR_LEVEL, "Buffer/Object cross validation failed [%x]", response);
			bufToString((unsigned char *)tbuf, length, (unsigned char *)lstr, 1024 );
			logMessage(LOG_INFO_LEVEL, "CIO_UTEST VR: %s", lstr);
			bufToString((unsigned char *)cio_utest_buffer, length, (unsigned char *)lstr, 1024 );
			logMessage(LOG_INFO_LEVEL, "CIO_UTEST VU: %s", lstr);
			return(-1);
		}

		// Print out the buffer
		bufToString((unsigned char *)cio_utest_buffer, cio_utest_length, (unsigned char *)lstr, 1024 );
		logMessage(LOG_INFO_LEVEL, "CIO_UTEST: %s", lstr);
#endif

	}

	// Close the files and cleanup buffers, assert on failure
	if (crud_close(fh)) {
		logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Failure read comparison block.", fh);
		return(-1);
	}
	free(cio_utest_buffer);
	free(tbuf);

	// Return successfully
	return(0);
}

































