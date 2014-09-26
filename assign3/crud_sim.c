////////////////////////////////////////////////////////////////////////////////
//
//  File           : crudsim.c
//  Description    : This is the main program for the CMPSC311 programming
//                  assignment #3 (beginning of CRUD interface).
//
//   Author        : Patrick McDaniel
//   Last Modified : Tue Sep 23 07:13:21 EDT 2014
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

// Project Includes
#include <crud_driver.h>
#include <crud_file_io.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <cmpsc311_hashtable.h>

// Defines
#define CRUD_ARGUMENTS "hvul:"
#define USAGE \
	"USAGE: crud [-h] [-v] [-l <logfile>] [-c <sz>] <workload-file>\n" \
	"\n" \
	"where:\n" \
	"    -h - help mode (display this message)\n" \
	"    -u - run the unit tests instead of the simulator\n" \
	"    -v - verbose output\n" \
	"    -l - write log messages to the filename <logfile>\n" \
	"\n" \
	"    <workload-file> - file contain the workload to simulate\n" \
	"\n" \

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
	int ch, verbose = 0, unit_tests = -0, log_initialized = 0;
	uint32_t cache_size = 1024; // Defaults to 1024 cache lines

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

		case 'c': // Set cache line size
			if ( sscanf( optarg, "%u", &cache_size ) != 1 ) {
			    logMessage( LOG_ERROR_LEVEL, "Bad  cache size [%s]", argv[optind] );
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
		if ( crudIOUnitTest() ) {
			logMessage( LOG_ERROR_LEVEL, "CRUD unit tests failed.\n\n" );
		} else {
			logMessage( LOG_INFO_LEVEL, "CRUD unit tests completed successfully.\n\n" );
		}

	} else {

		// The filename should be the next option
		if ( optind >= argc ) {

			// No filename
			fprintf( stderr, "Missing command line parameters, use -h to see usage, aborting.\n" );
			return( -1 );

		}

		// Run the simulation
        logMessage( LOG_INFO_LEVEL, "CRUD simulation not used for this assignment [PLEASE RUN UNIT TEST] .\n\n" );
	}

	// Return successfully
	return( 0 );
}
