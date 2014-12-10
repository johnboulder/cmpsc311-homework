////////////////////////////////////////////////////////////////////////////////
//
//  File           : crud_file_io.h
//  Description    : This is the implementation of the standardized IO functions
//                   for used to access the CRUD storage system.
//
//  Author         : John Walter Stockwell
//  Last Modified  : Sun Nov 16 10:23PM
//

// Includes
#include <malloc.h>
#include <string.h>

// Project Includes
#include <crud_file_io.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines
#define CIO_UNIT_TEST_MAX_WRITE_SIZE 1024
#define CRUD_IO_UNIT_TEST_ITERATIONS 10240

// Student definitions and structures
#define FILE_HANDLE_BASE_VALUE 0

int8_t crud_initialized = 0;
int16_t current_handle = FILE_HANDLE_BASE_VALUE;

// Struct intended for interacting with the object store
typedef struct 
{
	// After exitting functions, we can only hold the file descriptor, position, and OID
	uint32_t oid;
	int8_t request; // Non-global
	int32_t length; // Non-global
	int8_t flags; // Non-global
	int8_t result; // Non-global
	uint32_t position;
} file_st;

/* Prototypes for some extra helper functions at the bottom of this class.
 * Used for a variety of things. See there individual comments for more 
 * details 
 */
CrudRequest createRequest(int32_t oid, int8_t request, int32_t length, int8_t flags);
file_st processResponse(CrudResponse res, int16_t fd);
int16_t getNewHandle();
void crud_init();
CrudFileAllocationType convertToCrudFileType(file_st file, int16_t fd, char* filename);


// Type for UNIT test interface
typedef enum {
	CIO_UNIT_TEST_READ   = 0,
	CIO_UNIT_TEST_WRITE  = 1,
	CIO_UNIT_TEST_APPEND = 2,
	CIO_UNIT_TEST_SEEK   = 3,
} CRUD_UNIT_TEST_TYPE;

CrudFileAllocationType crud_file_table[CRUD_MAX_TOTAL_FILES];

// Pick up these definitions from the unit test of the crud driver
/* NOTE:
 * I opted not to use these functions in my implementation because I thought it'd turn out to be easier to just not 
 * bother with them at all.
 */
CrudRequest construct_crud_request(CrudOID oid, CRUD_REQUEST_TYPES req, uint32_t length, uint8_t flags, uint8_t res);
int deconstruct_crud_request(CrudRequest request, CrudOID *oid, CRUD_REQUEST_TYPES *req, uint32_t *length, uint8_t *flags, uint8_t *res);

/////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_format
// Description  : This function formats the crud drive, and adds the file
//                allocation table.
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

uint16_t crud_format(void) {

	// Ensure that crud is initialized
	if(!crud_initialized)
		crud_init();

	// Format the store
	CrudRequest req = createRequest(0, CRUD_FORMAT, 0, 0);
	CrudResponse res = crud_client_operation(req, NULL);
	file_st local_file = processResponse(res, -1);
	
	// Check that the response did not include a failure
	if(local_file.result == 1)
		return -1;

	// Initialize the table entries
	CrudFileAllocationType new_table[CRUD_MAX_TOTAL_FILES]= {[0 ... CRUD_MAX_TOTAL_FILES-1] = {"empty",0,0,0,0} };
	memcpy(crud_file_table, new_table, sizeof(CrudFileAllocationType) * CRUD_MAX_TOTAL_FILES);
	
	// Initialize the priority object
	req = createRequest(0, CRUD_CREATE, CRUD_MAX_TOTAL_FILES*sizeof(CrudFileAllocationType), CRUD_PRIORITY_OBJECT);
	res = crud_client_operation(req, crud_file_table);
	local_file = processResponse(res, -1);
	
	// Zero out the file handle
	current_handle = FILE_HANDLE_BASE_VALUE;

	if(local_file.result == 1)
		return -1;

	// Log, return successfully
	logMessage(LOG_INFO_LEVEL, "... formatting complete.");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_mount
// Description  : This function mount the current crud file system and loads
//                the file allocation table.
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

uint16_t crud_mount(void) {

	// Ensure that crud is initialize
	if(!crud_initialized)
		crud_init();

	// Initialize an empty buffer
	CrudFileAllocationType buf[CRUD_MAX_TOTAL_FILES];

	// Request the priority object from the file store
	CrudRequest req = createRequest(0, CRUD_READ, CRUD_MAX_TOTAL_FILES*sizeof(CrudFileAllocationType), CRUD_PRIORITY_OBJECT);
	CrudResponse res = crud_client_operation(req, buf);
	file_st local_file = processResponse(res, -1);
	
	// Check that the response did not include a failure
	if(local_file.result == 1)
		return -1;

	// Copy the buffer to the file table
	memcpy(crud_file_table, buf, sizeof(CrudFileAllocationType) * CRUD_MAX_TOTAL_FILES);
	
	// Initialize the file handle in case entries exist in crud_file_table
	int i = 0; 
	while(i<CRUD_MAX_TOTAL_FILES && strcmp(crud_file_table[i].filename,"empty"))
	{
		i+=1;
	}
	current_handle = i;

	// Log, return successfully
	logMessage(LOG_INFO_LEVEL, "... mount complete.");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_unmount
// Description  : This function unmounts the current crud file system and
//                saves the file allocation table.
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

uint16_t crud_unmount(void) {

	if(!crud_initialized)
		crud_init();
	
	// Update the priority object
	CrudRequest req = createRequest(0, CRUD_UPDATE, CRUD_MAX_TOTAL_FILES*sizeof(CrudFileAllocationType), CRUD_PRIORITY_OBJECT);
	CrudResponse res = crud_client_operation(req, crud_file_table);
	file_st local_file = processResponse(res, -1);
	
	if(local_file.result == 1)
		return -1;

	// Close all the shit
	req = createRequest(0, CRUD_CLOSE, 0, 0);
	res = crud_client_operation(req, crud_file_table);
	local_file = processResponse(res, -1);

	if(local_file.result == 1)
		return -1;

	// Log, return successfully
	logMessage(LOG_INFO_LEVEL, "... unmount complete.");
	return (0);
}

// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_open
// Description  : This function opens the file and returns a file handle
//
// Inputs       : path - the path "in the storage array"
// Outputs      : file handle if successful, -1 if failure

int16_t crud_open(char *path) 
{	
	// Ensure that crud is initialized
	if(!crud_initialized)
		crud_init();

	// Return the file handle of the file if the file already exists in crud_file_table
	int16_t i = 0;
	for(i = 0; i<=CRUD_MAX_TOTAL_FILES; i++)
	{
		if(strcmp(crud_file_table[i].filename, path) == 0)
		{
			crud_file_table[i].open = 1;
			crud_file_table[i].position = 0;
			return i;
		}
	}

	// Create a new file on the store if it doesn't exist
	CrudRequest req = createRequest(0, CRUD_CREATE, 0, 0);
	CrudResponse res = crud_client_operation(req, NULL);
	file_st local_file = processResponse(res, -1);
	
	// Check that the response did not include a failure
	if(local_file.result == 1)
		return -1;

	int16_t handle = getNewHandle();
	convertToCrudFileType(local_file, handle, path);

	return handle;
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
	// Only thing we set to zero is the open flag
	crud_file_table[fh].open = 0;

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
	//logMessage(LOG_INFO_LEVEL, "count:%d length:%d position:%d filename:%s ", count, crud_file_table[fd].length, crud_file_table[fd].position, crud_file_table[fd].filename);
	if(!crud_initialized)
		crud_init();

	if(!crud_file_table[fd].open)
		return -1;

	// Declare a new char buffer for use later
	unsigned char tmpBuf[CRUD_MAX_OBJECT_SIZE];

	// Create a request to send to the crud bus
	CrudRequest req = createRequest(crud_file_table[fd].object_id, CRUD_READ, CRUD_MAX_OBJECT_SIZE, 0);
	CrudResponse res = crud_client_operation(req, tmpBuf);
	file_st local_file = processResponse(res, fd);
	
	if(local_file.result == 1)
		return -1;

	// Check to ensure we're not reading off the end of our file
	if(count+local_file.position > local_file.length)
		count = local_file.length - local_file.position;

	// TODO do a memcpy instead?
	int i = 0;
	// Loop through the buffer that we read, assigning each position in it to the buffer
	// we were passed
	for( i = local_file.position; i<local_file.position+count; i++)
		((unsigned char*)buf)[i-local_file.position] = tmpBuf[i];

	local_file.position+=count;

	// Convert and assign our file
	convertToCrudFileType(local_file, fd, crud_file_table[fd].filename);
	
	//printf("%s\n", (unsigned char*)buf);
	//logMessage(LOG_INFO_LEVEL, "count:%d length:%d position:%d filename:%s ", count, crud_file_table[fd].length, crud_file_table[fd].position, crud_file_table[fd].filename);

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
	char lstr[1024];
	bufToString((unsigned char *)buf, count, (unsigned char *)lstr, 1024 );
	//printf( "buffer in write: %s \n\n", lstr);

	unsigned char tmpBuf[CRUD_MAX_OBJECT_SIZE];
	//printf("%s\n", (unsigned char*)buf);
	if(!crud_initialized)
		crud_init();
	
	CrudFileAllocationType current_file = crud_file_table[fd];

	if(!current_file.open)
		return -1;

	// Given the fild handle, find it's OID and read the data from the store
	CrudRequest req = createRequest(current_file.object_id, CRUD_READ, CRUD_MAX_OBJECT_SIZE, 0);
	CrudResponse res = crud_client_operation(req, tmpBuf);
	file_st local_file = processResponse(res, fd);

	if(local_file.result == 1)
		return -1;

	// Ensures that we're not writing off the end of our file.
	// If the count goes past the file length, we reassign our file length
	// to the count + our current position. And then create a file of that
	// size. After we delete our old file of course.
	if(count+local_file.position > local_file.length)
	{
		int32_t length = count+local_file.position;
		if(length>CRUD_MAX_OBJECT_SIZE)
			length = CRUD_MAX_OBJECT_SIZE;

		req = createRequest(local_file.oid, CRUD_DELETE, 0, 0);
		int32_t tmpPos = local_file.position;

		crud_client_operation(req, NULL);

		req = createRequest(0, CRUD_CREATE, length, 0);
		res = crud_client_operation(req, tmpBuf);
		file_st new_file = processResponse(res, fd);

		local_file = new_file;
		local_file.position = tmpPos;
	}
	
	// Loop through, assigning positions in our temp buffer to positions in our 
	// passed buffer.
	int i = 0;
	for( i = 0; i<count; i++)
	{
		tmpBuf[local_file.position+i] = ((unsigned char*)buf)[i];
	}
	//int32_t tmpPos = local_file.position;
	
	// Update our file with the temporary buffer
	req = createRequest(local_file.oid, CRUD_UPDATE, local_file.length, 0);
	res = crud_client_operation(req, tmpBuf);
	local_file = processResponse(res, fd);
	//local_file.position = tmpPos;
	
	// Update our position
	local_file.position+=count;

	// Update our file
	convertToCrudFileType(local_file, fd, crud_file_table[fd].filename);

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
	if(!crud_initialized)
		crud_init();

	CrudFileAllocationType local_file = crud_file_table[fd];

	if(!local_file.open)
		return -1;

	// Seek to the shit
	local_file.position = (int32_t)loc;

	crud_file_table[fd] = local_file;

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_init
// Description  : initialize the crud store
//
// Inputs       : Nothin!
// Outputs      : Nothin! void.

void crud_init()
{
	void *buf = NULL;
	CrudRequest req = createRequest(0, CRUD_INIT, 0, 0);
	CrudResponse res = crud_client_operation(req, buf);
	file_st file = processResponse(res, -1);
	
	if(file.result)
	{
		logMessage(LOG_INFO_LEVEL, "Crud init failed.");
		return;
	}

	logMessage(LOG_INFO_LEVEL, "CRUD Initialized");
	crud_initialized = 1;
	return;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : createRequest
// Description  : Pretty self explanatory. Simply creates a 64bit request to be
// 		  passed on to the object store
//
// Inputs       : 32 bit OID integer, 8bit request integer, and 32bit length integer
//
// Outputs      : 0 if successful or -1 if failure
CrudRequest createRequest(int32_t oid, int8_t request, int32_t length, int8_t flags)
{
	CrudRequest req = 0;
	
	// Add the 32 bits of oid to the 64 least significant bits of req
	req+= oid;
	req<<=4;
	// Add the however many from the request and length
	req+= (15 & request);
	req<<=24;

	req+= (16777215 & length);
	req<<=3;

	req+= (int8_t)( 7 & flags );
	req<<=1;
	
	return req;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : processResponse
// Description  : does all the bitshifting and other bullshit involved in taking 
// 		  a 64bit response and separating its parts out
//
// Inputs       : A response in the form of the CrudResponse struct provided in the 
// 		  driver or whatever
//
// Outputs      : A file struct 
file_st processResponse(CrudResponse res, int16_t fd)
{
	file_st file;
	file.result = (int8_t)( 1 & res );
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
	
	if(fd>=0)
		file.position = crud_file_table[fd].position;
	else
		file.position = 0;

	return file;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : getNewHandle
// Description  : Gets the next file handle
//
// Inputs       : Nothin!
// Outputs      : Returns the next available file handle
int16_t getNewHandle()
{
	return current_handle++;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : convertToCrudFileType
// Description  : Helper function converts file_st to CrudFileAllocationType
//
// Inputs       : file_st struct to be converted to CrudFileAllocationType
// Outputs      : CrudFileAllocationType file
CrudFileAllocationType convertToCrudFileType(file_st file, int16_t fd, char* filename)
{
	CrudFileAllocationType newFile = {"empty", 0, 0, 0, 0};
	newFile.object_id = file.oid;
	newFile.position = file.position;
	newFile.length = file.length;
	newFile.open = 1;
	strcpy(newFile.filename, filename);
	crud_file_table[fd] = newFile;

	return newFile;
}

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

	// Format and mount the file system
	if (crud_format() || crud_mount()) {
		logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Failure on format or mount operation.");
		return(-1);
	}

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
				
				//logMessage(LOG_INFO_LEVEL, "unit test position: %d", cio_utest_position);
				
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
				
				bufToString((unsigned char *)&cio_utest_buffer[cio_utest_position], bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "buffer in test: %s", lstr);

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
		request = construct_crud_request(crud_file_table[0].object_id, CRUD_READ, CRUD_MAX_OBJECT_SIZE, CRUD_NULL_FLAG, 0);
		response = crud_client_operation(request, tbuf);
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

	// Format and mount the file system
	if (crud_unmount()) {
		logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Failure on unmount operation.");
		return(-1);
	}

	// Return successfully
	return(0);
}

































