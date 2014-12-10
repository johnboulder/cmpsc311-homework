////////////////////////////////////////////////////////////////////////////////
//
//  File          : crudsim.c
//  Description   : This is the main program for the CMPSC311 programming
//                  assignment #3 (beginning of CRUD interface).
//
//   Author : Patrick McDaniel
//   Last Modified : Sat Sep  6 08:24:25 EDT 2014
//

// Include Files
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Project Includes
#include <crud_driver.h>
#include <crud_network.h>
#include <crud_file_io.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines
#define CRUD_SIM_MAX_OPEN_FILES 128
#define CRUD_ARGUMENTS "hvul:x:a:p:"
#define USAGE \
	"USAGE: crud [-h] [-v] [-l <logfile>] [-c <sz>] [-x <file>] [-a <ip addr>] [-p <port>] <workload-file>\n" \
	"\n" \
	"where:\n" \
	"    -h - help mode (display this message)\n" \
	"    -u - run the unit tests instead of the simulator\n" \
	"    -v - verbose output\n" \
	"    -l - write log messages to the filename <logfile>\n" \
	"    -x - extract a file <file> from the crud filesystem\n" \
	"    -a - IP address of server to connect to.\n" \
	"    -p - port number of server to connect to.\n" \
	"\n" \
	"    <workload-file> - file contain the workload to simulate\n" \
	"\n" \

// This is the file table
typedef struct {
	char     *filename;  // This is the filename for the test file
	int16_t   fhandle;   // This is a file handle for the opened file
} CrudSimulationTable;

//
// Global Data
int verbose;

//
// Functional Prototypes

int simulate_CRUD( char *wload );
int extract_file_from_crud(char *ex_file);

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the CRUD simulator
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful test, -1 if failure

int main( int argc, char *argv[] ) {
	// Local variables
	int ch, verbose = 0, unit_tests = 0, log_initialized = 0, extract_file = 0;
	uint32_t cache_size = 1024; // Defaults to 1024 cache lines
	char *ex_file = NULL;

	// Process the command line parameters
	while ((ch = getopt(argc, argv, CRUD_ARGUMENTS)) != -1) {

		switch (ch) {
		case 'h': // Help, print usage
			fprintf( stderr, USAGE );
			return( -1 );

		case 'v': // Verbose Flag
			verbose = 1;
			break;

		case 'u': // Unit Tests Flag
			unit_tests = 1;
			break;

		case 'l': // Set the log filename
			initializeLogWithFilename( optarg );
			log_initialized = 1;
			break;

		case 'x': // Set the log filename
			ex_file = optarg;
			extract_file = 1;
			break;

		case 'c': // Set cache line size
			if ( sscanf( optarg, "%u", &cache_size ) != 1 ) {
			    logMessage( LOG_ERROR_LEVEL, "Bad  cache size [%s]", argv[optind] );
                return(-1);
			}
			break;

        case 'a': // Get the IP address
            if (inet_addr(optarg) == INADDR_NONE) {
			    logMessage( LOG_ERROR_LEVEL, "Bad  cache size [%s]", argv[optind] );
                return(-1);
            } 
            crud_network_address = (unsigned char *)strdup(optarg);
			break;

        case 'p': // Set the network port number
			if ( sscanf(optarg, "%hu", &crud_network_port) != 1 ) {
			    logMessage( LOG_ERROR_LEVEL, "Bad  port number [%s]", argv[optind] );
                return(-1);
			}
            break;

		default:  // Default (unknown)
			fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
			return( -1 );
		}
	}

	// Setup the log as needed
	if ( ! log_initialized ) {
		initializeLogWithFilehandle( CMPSC311_LOG_STDERR );
	}
	if ( verbose ) {
		enableLogLevels( LOG_INFO_LEVEL );
	}

	// If we are running the unit tests, do that
	if ( unit_tests ) {

		// Enable verbose, run the tests and check the results
		enableLogLevels( LOG_INFO_LEVEL );
		if ( b64UnitTest() || crudIOUnitTest() ) {
			logMessage( LOG_ERROR_LEVEL, "CRUD unit tests failed.\n\n" );
		} else {
			logMessage( LOG_INFO_LEVEL, "CRUD unit tests completed successfully.\n\n" );
		}

	} else if (extract_file) {

		// Extracting a file from the crud file systems
		if (extract_file_from_crud(ex_file) == 0) {
			logMessage(LOG_INFO_LEVEL, "File [%s] extracted from crud successfully.\n\n", ex_file);
		} else {
			logMessage(LOG_ERROR_LEVEL, "File [%s] extraction failed, aborting.\n\n");
		}

	} else {

		// The filename should be the next option
		if ( optind >= argc ) {

			// No filename
			fprintf( stderr, "Missing command line parameters, use -h to see usage, aborting.\n" );
			return( -1 );

		}

		// Run the simulation
		if ( simulate_CRUD(argv[optind]) == 0 ) {
			logMessage( LOG_INFO_LEVEL, "CRUD simulation completed successfully.\n\n" );
		} else {
			logMessage( LOG_INFO_LEVEL, "CRUD simulation failed.\n\n" );
		}
	}

	// Return successfully
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : simulate_CRUD
// Description  : The main control loop for the processing of the CRUD
//                simulation.
//
// Inputs       : wload - the name of the workload file
// Outputs      : 0 if successful test, -1 if failure

int simulate_CRUD( char *wload ) {

	// Local variables
	char line[2048], fname[128], command[128], text[2048], *sep, *rbuf;
	FILE *fhandle = NULL;
	int32_t err=0, len, off, fields, linecount;
	CrudSimulationTable ftable[CRUD_SIM_MAX_OPEN_FILES];
	int idx, i;

	// Setup the file table
	memset(ftable, 0x0, sizeof(CrudSimulationTable)*CRUD_SIM_MAX_OPEN_FILES);

	// Open the workload file
	linecount = 0;
	if ( (fhandle=fopen(wload, "r")) == NULL ) {
		logMessage( LOG_ERROR_LEVEL, "Failure opening the workload file [%s], error: %s.\n",
			wload, strerror(errno) );
		return( -1 );
	}

	// While file not done
	while (!feof(fhandle)) {

		// Get the line and bail out on fail
		if (fgets(line, 2048, fhandle) != NULL) {

			// Parse out the string
			linecount ++;
			fields = sscanf(line, "%s %s %d %d", fname, command, &len, &off);
			sep = strchr(line, ':');
			if ( (fields != 4) || (sep == NULL) ) {
				logMessage( LOG_ERROR_LEVEL, "CRUD un-parsable workload string, aborting [%s], line %d",
						line, linecount );
				fclose( fhandle );
				return( -1 );
			}

			// Just log the contents
			logMessage(LOG_INFO_LEVEL, "File [%s], command [%s], len=%d, offset=%d",
					fname, command, len, off);

			// Now process the commands
			if (strncmp(command, "FORMAT", 6) == 0) {

				// Log the command executed
				logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Formatting CRUD filesystem");

				// Now perform the format
				if (crud_format() != len) {
					// Failed, error out
					logMessage(LOG_ERROR_LEVEL, "Formatting failed, aborting simulation.");
					return(-1);
				}

			} else if (strncmp(command, "MOUNT", 5) == 0) {

				// Log the command executed
				logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Mounting CRUD filesystem");

				// Now perform the filesystem mount
				if (crud_mount() != len) {
					// Failed, error out
					logMessage(LOG_ERROR_LEVEL, "Mount failed, aborting simulation.");
					return(-1);
				}

			} else if (strncmp(command, "UNMOUNT", 5) == 0) {

				// Log the command executed
				logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Un-mounting CRUD filesystem");

				// Finished, close all of the files
				for (idx=0; idx<CRUD_SIM_MAX_OPEN_FILES; idx++) {

					// If file in use, close if
					if (ftable[idx].filename != NULL) {
						// Log the file close
						logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Closing file [%s]", ftable[idx].filename);
						if (crud_close(ftable[idx].fhandle) == -1) {
							// Failed, error out
							logMessage(LOG_ERROR_LEVEL, "Close file [%s] failed, aborting simulation.", ftable[idx].filename);
							return(-1);
						}
						free(ftable[idx].filename);
						ftable[idx].filename = NULL;
					}

				}

				// Now perform the filesystem unmount
				if (crud_unmount() != len) {
					// Failed, error out
					logMessage(LOG_ERROR_LEVEL, "Mount failed, aborting simulation.");
					return(-1);
				}


			} else {

				//
				// File operations

				// Now walk the the table looking for the file
				idx = -1;
				i = 0;
				while ( (i < CRUD_SIM_MAX_OPEN_FILES) && (idx == -1) ) {
					if ( (ftable[i].filename != NULL) && (strcmp(ftable[i].filename,fname) == 0) ) {
						idx = i;
					}
					i++;
				}

				// File is not found, open the file
				if (idx == -1) {

					// Log message, find unused index and save filename for later use
					logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Opening file [%s]", fname);
					idx = 0;
					while ((ftable[idx].filename != NULL) && (idx < CRUD_SIM_MAX_OPEN_FILES)) {
						idx++;
					}
					CMPSC_ASSERT1(idx<CRUD_SIM_MAX_OPEN_FILES, "Too many open files on CRUD sim [%d]", idx);
					ftable[idx].filename = strdup(fname);

					// Now perform the open
					ftable[idx].fhandle = crud_open(ftable[idx].filename);
					if (ftable[idx].fhandle == -1) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "Open of new file [%s] failed, aborting simulation.", fname);
						return(-1);
					}

				}

				// Now execute the specific command
				if (strncmp(command, "WRITEAT", 7) == 0) {

					// Log the command executed
					logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Writing %d bytes at position %d from file [%s]", len, off, fname);

					// First perform the seek
					if (crud_seek(ftable[idx].fhandle, off)) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "Seek/WriteAt file [%s] to position %d failed, aborting simulation.", fname, off);
						return(-1);
					}

					// Now see if we need more data to fill, terminate the lines
					CMPSC_ASSERT1(len<1024, "Simulated workload command text too large [%d]", len);
					CMPSC_ASSERT2((strlen(sep+1)>=len), "Workload str [%d<%d]", strlen(sep+1), len);
					strncpy(text, sep+1, len);
					text[len] = 0x0;
					for (i=0; i<strlen(text); i++) {
						if (text[i] == '*') {
							text[i] = '\n';
						}
					}

					// Now perform the write
					if (crud_write(ftable[idx].fhandle, text, len) != len) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "WriteAt of file [%s], length %d failed, aborting simulation.", fname, len);
						return(-1);
					}

				} else if (strncmp(command, "WRITE", 5) == 0) {

					// Now see if we need more data to fill, terminate the lines
					CMPSC_ASSERT1(len<1024, "Simulated workload command text too large [%d]", len);
					CMPSC_ASSERT2((strlen(sep+1)>=len), "Workload str [%d<%d]", strlen(sep+1), len);
					strncpy(text, sep+1, len);
					text[len] = 0x0;
					for (i=0; i<strlen(text); i++) {
						if (text[i] == '*') {
							text[i] = '\n';
						}
					}

					// Log the command executed
					logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Writing %d bytes to file [%s]", len, fname);

					// Now perform the write
					if (crud_write(ftable[idx].fhandle, text, len) != len) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "Write of file [%s], length %d failed, aborting simulation.", fname, len);
						return(-1);
					}

				} else if (strncmp(command, "SEEK", 4) == 0) {

					// Log the command executed
					logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Seeking to position %d in file [%s]", off, fname);

					// Now perform the seek
					if (crud_seek(ftable[idx].fhandle, off) != len) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "Seek in file [%s] to position %d failed, aborting simulation.", fname, off);
						return(-1);
					}

				} else if (strncmp(command, "READ", 4) == 0) {

					// Log the command executed
					logMessage(LOG_INFO_LEVEL, "CRUD_SIM : Reading %d bytes from file [%s]", len, fname);

					// Now perform the read
					rbuf = malloc(len);
					if (crud_read(ftable[idx].fhandle, rbuf, len) != len) {
						// Failed, error out
						logMessage(LOG_ERROR_LEVEL, "Read file [%s] of length %d failed, aborting simulation.", fname, off);
						return(-1);
					}
					free(rbuf);
					rbuf = NULL;

				} else {

					// Bomb out, don't understand the command
					CMPSC_ASSERT1(0, "CRUD_SIM : Failed, unknown command [%s]", command);

				}
			}

			// Check for the virtual level failing
			if ( err ) {
				logMessage( LOG_ERROR_LEVEL, "CRUS system failed, aborting [%d]", err );
				fclose( fhandle );
				return( -1 );
			}
		}
	}

	// Close the workload file, successfully
	fclose( fhandle );
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : extract_file_from_crud
// Description  : Extract a file from the CRUD file system
//
// Inputs       : ex_file - the name of the file to extract
// Outputs      : 0 if successful test, -1 if failure

int extract_file_from_crud(char *ex_file) {

	// Local variables
	int16_t fd;
	int32_t len;
	char buf[CRUD_MAX_OBJECT_SIZE];
    int fhandle, flags;
    mode_t mode;
	// Open the file, read from it, close it
	if ( (crud_mount()) || ((fd = crud_open(ex_file)) == -1) ||
		 ((len = crud_read(fd, buf, CRUD_MAX_OBJECT_SIZE)) == -1) ||
		 (crud_close(fd) == -1)	) {
		// Error out
		logMessage(LOG_INFO_LEVEL, "CRUD : extraction failed on crud interface [%s].", ex_file);
		return(-1);
	}

    // Setup the file for creating and open
    flags = O_WRONLY|O_CREAT|O_EXCL; // Create a NEW file (no overwrite)
    mode = S_IRUSR|S_IWUSR|S_IRGRP;   // User can read/write, group read
    fhandle = open(ex_file, flags, mode);
    if ( fhandle == -1 ) {
        fprintf( stderr, "CRUD: extraction open() failed, error=%s\n", strerror(errno) );
        return( -1 );
    }

    // Now write the read bytes to the file, then close
    if (write(fhandle, buf, len) != len) {
        fprintf( stderr, "CRUD: extraction write() failed, error=%s\n", strerror(errno) );
        return( -1 );
    }
    close( fhandle );

    // Return successfully
	return( 0 );
}
