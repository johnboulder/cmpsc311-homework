////////////////////////////////////////////////////////////////////////////////
//
//  File          : crud_client.c
//  Description   : This is the client side of the CRUD communication protocol.
//
//   Author       : John Stockwell
//  Last Modified : Wed Dec 10 12:49 EDT 2014
//

// Include Files

// Project Include Files
#include <crud_network.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Global variables
int            crud_network_shutdown = 0; // Flag indicating shutdown
unsigned char *crud_network_address = NULL; // Address of CRUD server 
unsigned short crud_network_port = 0; // Port of CRUD server
struct         sockaddr_in v4; // IPV4 address
int            socket_fd = 0; // Socket file descriptor
int            connected = 0; // Connected flag
int64_t        length = 0; // Number of bytes to send or read

// Functions
int64_t getRequest(CrudRequest res);
int     establishConnection();
int64_t receive(CrudRequest req, void *buf);
int64_t send1(CrudRequest req, void *buf);

////////////////////////////////////////////////////////////////////////////////
//
// Function     : crud_client_operation
// Description  : This the client operation that sends a request to the CRUD
//                server.   It will:
//
//                1) if INIT make a connection to the server
//                2) send any request to the server, returning results
//                3) if CLOSE, will close the connection
//
// Inputs       : op - the request opcode for the command
//                buf - the block to be read/written from (READ/WRITE)
// Outputs      : the response structure encoded as needed

CrudResponse crud_client_operation(CrudRequest op, void *buf) 
{

	/* Some debug output.
	 * switch(getRequest(op))
	{
		case CRUD_INIT:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_INIT called");
			break;
		case CRUD_FORMAT:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_FORMAT called");
			break;
		case CRUD_CREATE:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_CREATE called");
			break;
		case CRUD_READ:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_READ called");
			break;
		case CRUD_UPDATE:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_UPDATE called");
			break;
		case CRUD_DELETE:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_DELETE called");
			break;
		case CRUD_CLOSE:
			logMessage(LOG_INFO_LEVEL, "----------------------------------------");
			logMessage(LOG_INFO_LEVEL, "CRUD_CLOSE called");
			break;
	};*/

	CrudResponse res = 0;
	res = send1(op, buf);
	if(res < 0)
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client_operation : send failed");
		return -1;
	}
	
	res = receive(op, buf);
	if(res < 0)
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client_operation : receive failed");
		return -1;
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////
////
//// Function     : establishConnection
//// Description  : Just connects to the server and updates the connected flag
////
//// Inputs       : Nothing
////
//// Outputs      : 0 if successful, -1 if unsuccessful
int establishConnection()
{

	inet_aton(CRUD_DEFAULT_IP,  &(v4.sin_addr));
	v4.sin_port = htons(CRUD_DEFAULT_PORT);
	v4.sin_family = AF_INET;

	socket_fd = socket(PF_INET, SOCK_STREAM, 0);

	if( connect(socket_fd, (const struct sockaddr *) &(v4),  sizeof(v4)) == -1)
		return -1;

	connected = 1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////
//// Function     : receive
//// Description  : performs a read on on the server currently connected to and
//// 		    passes and data into buf
////
//// Inputs       : req - A standard 64bit crud request
////		    buf - A void pointer we'll be passing data into
//// Outputs      : Returns a standard 64bit response value or -1 if it fails
int64_t receive(CrudRequest req, void *buf)
{

	if(!connected)
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client.c : receive method called but not connected");
		return -1;
	}

	CrudResponse res = req;
	res = htonll64(res);
	int n = read( socket_fd, &res, sizeof(res));
	int tmpLength = 0;

	if( n != sizeof(res) )
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client.c : failed read from server");
		return -1;
	}

	res = ntohll64(res);
	
	if(getRequest(res) == CRUD_READ)
	{
		tmpLength = length;
		do{

			n = read( socket_fd, buf, tmpLength );

			if( n < 0 )
			{
				logMessage(LOG_ERROR_LEVEL, "crud_client.receive() : failed read from server");
				return -1;
			}

			tmpLength -= n;
			buf+=n;

		}while(  n>0 );
		
	}

	return res;

}

////////////////////////////////////////////////////////////////////////////////
////
//// Function     : send1
//// Description  : Performs a write to the server currently connected to
////
//// Inputs       : req - A standard 64bit crud request
////		    buf - A void pointer we'll be passing data into
//// Outputs      : Returns a standard 64bit response value or -1 if it fails
int64_t send1(CrudRequest req, void *buf)
{

	if(!connected)
		if(establishConnection())
			return -1;

	CrudRequest tmpReq = htonll64(req);

	int n = write( socket_fd, &tmpReq, sizeof(tmpReq) );

	if( n != sizeof(tmpReq) )
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client.send() : failed write to server");
		return -1;
	}

	tmpReq = ntohll64(tmpReq);

	//logMessage(LOG_INFO_LEVEL, "crud_client.send() : sent %d bytes", n);

	if(getRequest(req) == CRUD_CREATE || getRequest(req) == CRUD_UPDATE)
	{
		n = write( socket_fd, buf, length );
		//logMessage(LOG_INFO_LEVEL, "crud_client.send() : sent  %d bytes", n);

		if( n != length )
		{
			logMessage(LOG_ERROR_LEVEL, "crud_client.send() : failed write to server");
			return -1;
		}

	}

	return tmpReq;
}

////////////////////////////////////////////////////////////////////////////////
////
//// Function     : getRequest
//// Description  : Sets the global variable for length, and returns the 
////		    value of the request
//// Inputs       : req - A standard 64bit crud request
////
//// Outputs      : Returns a 64bit value for the request
int64_t getRequest(CrudRequest req)
{
	// Length
	req>>=4;
	
	length = (int64_t)(16777215 & req);
	req>>=24;
	
	return (int64_t)(15 & req);
}

