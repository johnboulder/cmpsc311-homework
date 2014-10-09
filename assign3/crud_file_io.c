////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_file_io.h
//  Description    : This is the implementation of the standardized IO functions
//                   for used to access the CRUD storage system.
//
//  Author         : John Walter Stockwell
//  Last Modified  : Tue Sep 16 19:38:42 EDT 2014
//

// Includes
#include <malloc.h>
#include <string.h>

// Project Includes
#include <crud_file_io.h>
#include <crud_driver.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines 
#define CIO_UNIT_TEST_MAX_WRITE_SIZE 1024
#define CRUD_IO_UNIT_TEST_ITERATIONS 10240
#define FILE_HANDLE_BASE_VALUE 16

int8_t crud_initialized = 0;
int16_t current_handle = FILE_HANDLE_BASE_VALUE;

// File structure.
// May in future become useful in keeping track of a shitload of concurrent files
typedef struct 
{
	// After exitting functions, we can only hold the file descriptor, position, and OID
	int32_t oid;
	int8_t request; // Non-global
	int32_t length; // Non-global
	int8_t flags; // Non-global
	int8_t result; // Non-global
	int32_t position;
	int16_t handle; // Known as fd in most functions
} file_st;

//TODO Should possibly include an implementation here for a linked list for later

file_st current_file = { 0, 0, 0, 0, 0, 0 };

// Type for UNIT test interface
typedef enum 
{
    CIO_UNIT_TEST_READ   = 0,
    CIO_UNIT_TEST_WRITE  = 1,
    CIO_UNIT_TEST_APPEND = 2,
    CIO_UNIT_TEST_SEEK   = 3,
} CRUD_UNIT_TEST_TYPE;

// Prototypes
CrudRequest createRequest(int32_t oid, int8_t request, int32_t length);
file_st processResponse(CrudResponse res);
int16_t getNewHandle();
void crud_init();

// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_open
// Description  : This function opens the file and returns a file handle
//
// Inputs       : path - the path "in the storage array"
// Outputs      : file handle if successful, -1 if failure

int16_t crud_open(char *path) 
{	// Ensure that crud is initialized
	if(!crud_initialized)
		crud_init();
	
	// Create our request
	CrudRequest req = createRequest(0, CRUD_CREATE, 0);
	// Send the request to the data store
	CrudResponse res = crud_bus_request(req, NULL);
	// Process the response
	current_file = processResponse(res);
	// Check that the response did not include a failure
	if(current_file.result == 1)
		return -1;
	
	current_file.handle = getNewHandle();

	current_file.length = 0;
	current_file.flags = 0; 
	current_file.result = 0;
	current_file.request = 0;

	return current_file.handle;
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
	current_file.position = 0;
	current_file.oid = 0;
	current_file.request = 0;
	current_file.length = 0;
	current_file.result = 0;
	current_file.flags = 0;
	current_file.handle = 0;

	return 0;
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
	//printf("start count:%d \n", count);
	if(!crud_initialized)
		crud_init();

	char tmpBuf[CRUD_MAX_OBJECT_SIZE];

	// TODO For next assignment, we'll have to actually use the file handle for something.
	// Here we aren't actually using it for anything.

	CrudRequest req = createRequest(current_file.oid, CRUD_READ, CRUD_MAX_OBJECT_SIZE);
	CrudResponse res = crud_bus_request(req, tmpBuf);
	current_file = processResponse(res);
	//printf("count of bytes to read: %d\n", count);

	//printf("length of file we're reading: %d\n", current_file.length);
	//printf("position in file before read: %d\n", current_file.position);

	if(current_file.result == 1)
		return -1;


	int i = 0;
	if(count+current_file.position > current_file.length)
	{
		//printf("if statement evaluated as true\n");
		count = current_file.length - current_file.position;
		//printf("New count after if: %d\n", count);
	}

	for( i = current_file.position; i<current_file.position+count; i++)
	{
		((char*)buf)[i-current_file.position] = tmpBuf[i];
	}

	current_file.position+=count;
	
	//printf("position in file after read: %d\n", current_file.position);
	
	//current_file.length = 0;
	current_file.flags = 0; 
	current_file.result = 0;
	current_file.request = 0;

	return count;
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
	// If count is past current end of file, write over the end of the file

	// Find file in heap that we're writing to.
	
	char tmpBuf[CRUD_MAX_OBJECT_SIZE];
	if(!crud_initialized)
		crud_init();
	
	//printf("count: %d\n", count);

	// Given the fild handle, find it's OID and read the data from the store
	CrudRequest req = createRequest(current_file.oid, CRUD_READ, CRUD_MAX_OBJECT_SIZE);
	CrudResponse res = crud_bus_request(req, tmpBuf);
	current_file = processResponse(res);
	//TODO delete non global values from current_file

	if(current_file.result == 1)
		return -1;

	// Does not account for when the file is already the max object length
	if(count+current_file.position > current_file.length)
	{
		//printf("File write size reassignment occurred\n");
		int32_t length = count+current_file.position;
		if(length>CRUD_MAX_OBJECT_SIZE)
			length = CRUD_MAX_OBJECT_SIZE;

		//printf("length: %d\n", length);

		req = createRequest(current_file.oid, CRUD_DELETE, current_file.length);
		int32_t tmpPos = current_file.position;

		crud_close(current_file.handle);
		crud_bus_request(req, NULL);

		//printf("count: %d\n", count);

		req = createRequest(0, CRUD_CREATE, length);
		res = crud_bus_request(req, tmpBuf);
		file_st new_file = processResponse(res);

		current_file = new_file;

		current_file.position = tmpPos;
	}
	
	int i = 0;
	for( i = 0; i<count; i++)
	{
		tmpBuf[current_file.position+i] = ((unsigned char*)buf)[i];
	}

	req = createRequest(current_file.oid, CRUD_UPDATE, current_file.length);
	res = crud_bus_request(req, tmpBuf);
	current_file = processResponse(res);
	
	current_file.position+=count;

	//printf("Position at the end of write: %d\n", current_file.position);

	current_file.length = 0;
	current_file.flags = 0; 
	current_file.result = 0;
	current_file.request = 0;

	return count;
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

	if(!crud_initialized)
		crud_init();
	
	current_file.position = (int32_t)loc;
	return 0;
}

void crud_init()
{
	void *buf = NULL;
	//CRUD_REQUEST_TYPES requestType = CRUD_INIT;
	CrudRequest req = createRequest(0, CRUD_INIT, 0);
	CrudResponse res = crud_bus_request(req, buf);
	file_st file = processResponse(res);
	
	if(!file.result)
	{
		//TODO write some log and error stuff here
	}

	crud_initialized = 1;
}

CrudRequest createRequest(int32_t oid, int8_t request, int32_t length)
{
	CrudRequest req = 0;
	
	// Add the 32 bits of oid to the 64 least significant bits of req
	req+= oid;
	req<<=4;

	req+= (15 & request);
	req<<=24;

	req+= (16777215 & length);
	req<<=3;

	//TODO confirm that we don't actually need to use flags or result bits in any requests
	req<<=1;
	
	return req;
}

file_st processResponse(CrudResponse res)
{
	/* TODO
	 * find out how to correctly create logs, and log an error for when the result code in response is 1 (failure)
	 */
	file_st file;
	file.result = (int8_t)( 1 & res );
	// TODO include check here for response code of 1
	res>>=1;
	//printf("result:%u\n",file.result);

	// Flags
	file.flags = (int8_t)( 7 & res );
	res>>=3;
	//printf("flags:%u\n",file.flags);

	// Length
	file.length = (int32_t)(16777215 & res);
	res>>=24;
	//printf("length:%u\n",file.length);

	// Request
	file.request = (int8_t)(15 & res);
	res>>=4;
	//printf("request:%u\n",file.request);

	// OID
	file.oid = (int32_t)(4294967295 & res);
	// No need to shift res again.
	//printf("oid:%u\n",file.oid);

	// Get the file handle
	file.handle = getNewHandle();
	//printf("handle:%u\n",file.handle);

	file.position = current_file.position;

	return file;
}

int16_t getNewHandle()
{
	return current_handle+=1;
}

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
			//printf("392 \n");
		} else {
			cmd = getRandomValue(CIO_UNIT_TEST_READ, CIO_UNIT_TEST_SEEK);
			//printf("395 \n");

		}

		// Execute the command
		switch (cmd) {

		case CIO_UNIT_TEST_READ: // read a random set of data
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "CRUD_IO_UNIT_TEST : read %d at position %d", bytes, cio_utest_position);
			//printf("Count in Unit Test: %d \n");
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

































