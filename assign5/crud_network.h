#ifndef CRUD_NETWORK_INCLUDED
#define CRUD_NETWORK_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : crud_network.h
//  Description   : This is the network definitions for  the CRUD simulator.
//
//  Author        : Patrick McDaniel
//  Last Modified : Thu Oct 30 06:59:59 EDT 2014
//

// Include Files

// Project Include Files
#include <crud_driver.h>

// Defines
#define CRUD_MAX_BACKLOG 5
#define CRUD_NET_HEADER_SIZE sizeof(CrudResponse)
#define CRUD_DEFAULT_IP "127.0.0.1"
#define CRUD_DEFAULT_PORT 19876

//
// Functional Prototypes

CrudResponse crud_client_operation(CrudRequest op, void *buf);
    // This is the implementation of the client operation (crud_client.c)

int crud_server( void );
    // This is the implementation of the server application (crud_server.c)

//
// Network Global Data

extern int            crud_network_shutdown; // Flag indicating shutdown
extern unsigned char *crud_network_address;  // Address of CRUD server 
extern unsigned short crud_network_port;     // Port of CRUD server

#endif
