////////////////////////////////////////////////////////////////////////////////
//
//  File          : crud_client.c
//  Description   : This is the client side of the CRUD communication protocol.
//
//   Author       : Patrick McDaniel
//  Last Modified : Thu Oct 30 06:59:59 EDT 2014
//

// Include Files

// Project Include Files
#include <crud_network.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <stdio.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

// Global variables
int            crud_network_shutdown = 0; // Flag indicating shutdown
unsigned char *crud_network_address = NULL; // Address of CRUD server 
unsigned short crud_network_port = 0; // Port of CRUD server
struct sockaddr_in v4;
int socket_fd = 0;
int connected = 0; 
int64_t length = 0;

// Functions
int64_t getRequest(CrudRequest res);
int establishConnection();
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

	/*
	 * Useful info:
	 * Server IP: CRUD_DEFAULT_IP
	 * Port: CRUD_DEFAULT_PORT
	 * When sending data: Must convert to "Network Byte Order"
	 * When receiving data: Must convert to "Host Byte Order"
	 * 
	 * Must do a disconnect after a crud_close occurs
	 */
	// int inet_aton(const char *addr, struct in_addr *inp);
	
	switch(getRequest(op))
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
	};

	/*TODO THE SERVER SEEMS TO BE SIMULATING ERRORS IN THE NETWORK, SO INCLUDE ERROR HANDLING
	 * DO A WHILE LOOP THAT RETRIES UNTIL AFTER A CERTAIN NUMBER OF ATTEMPTS*/


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
	int totalReadLength = 0;

	if( n != sizeof(res) )
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client.c : failed read from server");
		return -1;
	}

	res = ntohll64(res);
	logMessage(LOG_INFO_LEVEL, "crud_client.receive() : received value of %016X", (int64_t)res);

	if(getRequest(res) == CRUD_READ)
	{
		tmpLength = length;
		do{

			n = read( socket_fd, buf, tmpLength );
			logMessage(LOG_INFO_LEVEL, "crud_client.receive() : read  %d bytes", n);

			if( n < 0 )
			{
				logMessage(LOG_ERROR_LEVEL, "crud_client.receive() : failed read from server");
				return -1;
			}

			//logMessage(LOG_INFO_LEVEL, "buffer: %s",(unsigned char*)buf);

			totalReadLength+=n;
			tmpLength -= n;
			buf+=n;

		}while( /*tmpLength>0 &&*/ n>0 );
		
		//int i = 0;
		//for( i = 0; i<length; i++)
		//{
		//	((unsigned char*)buf)[i] = (unsigned char)ntohs(((unsigned char*)buf)[i]);
		//}

		logMessage(LOG_INFO_LEVEL, "totalReadLength: %d length: %d", totalReadLength, length);
		//assert(totalReadLength == length);
	}

	return res;

}

int64_t send1(CrudRequest req, void *buf)
{

	if(!connected)
		if(establishConnection())
			return -1;

	//if(getRequest(req) == CRUD_WRITE)
	//{
	//	char lstr[1024];
	//	bufToString((unsigned char*)buf, 
	//}

	CrudRequest tmpReq = htonll64(req);

	int n = write( socket_fd, &tmpReq, sizeof(tmpReq) );

	if( n != sizeof(tmpReq) )
	{
		logMessage(LOG_ERROR_LEVEL, "crud_client.send() : failed write to server");
		return -1;
	}

	tmpReq = ntohll64(tmpReq);

	logMessage(LOG_INFO_LEVEL, "crud_client.send() : sent %d bytes", n);

	if(getRequest(req) == CRUD_CREATE || getRequest(req) == CRUD_UPDATE)
	{
		/*int i = 0;
		for( i = 0; i<length; i++)
		{
			((unsigned char*)buf)[i] = (unsigned char)htons(((unsigned char*)buf)[i]);
		}*/

		n = write( socket_fd, buf, length );
		logMessage(LOG_INFO_LEVEL, "crud_client.send() : sent  %d bytes", n);

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
//// Function     : processResponse
//// Description  : does all the bitshifting and other bullshit involved in taking
////                a 64bit response and separating its parts out
////
//// Inputs       : A response in the form of the CrudResponse struct provided in the
////                driver or whatever
////
//// Outputs      : A file struct
int64_t getRequest(CrudRequest res)
{
	//file.result = (int8_t)( 1 & res );
	//res>>=1;
	//printf("result:%u\n",file.result);

	// Flags
	//file.flags = (int8_t)( 7 & res );
	//res>>=3;
	//printf("flags:%u\n",file.flags);

	// Length
	res>>=4;
	length = (int64_t)(16777215 & res);
	res>>=24;
	//printf("length:%u\n",file.length);

	// Request
	//file.request = (int8_t)(15 & res);
	//res>>=4;
	//printf("request:%u\n",file.request);

	// OID
	//file.oid = (int32_t)(4294967295 & res);
	// No need to shift res again.

	//if(fd>=0)
	//	file.position = crud_file_table[fd].position;
	//else
	//	file.position = 0;

	//return file;
	return (int64_t)(15 & res);
}

