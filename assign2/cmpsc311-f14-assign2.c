////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311-f14-assign2.c
//  Description   : This is the main code file for the CMPSC311 assignment 2.
//                  see class assignment for details on what needs to be added.
//
//  Author        : ???
//  Created       : ???
//
#include <stdio.h>
#include "a2support.h"
#include <math.h>
#define NUMBER_ENTRIES 20

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : This is the main function for the cmpsc311-f13-assign2 program.
//
// Inputs       : none
// Outputs      : 0 if successful, -1 otherwise

int main( void ) {

	// Local variables
	int integerArray[NUMBER_ENTRIES], i;
	char binaryStr[16];
	// Read the integer values
	for ( i=0; i<NUMBER_ENTRIES; i++ ) {
	    scanf( "%d", &integerArray[i] );
	}

	float floatArray[NUMBER_ENTRIES];
	for( i=0; i<NUMBER_ENTRIES; i++ )
	{
		if( i%2 == 0 )
			floatArray[i] = roundf(cosf((float) integerArray[i])*100) /100;
		else
			floatArray[i] = roundf(sinf((float) integerArray[i])*100) /100;
			
	}
	showFloats(floatArray, NUMBER_ENTRIES);
	showIntegers(integerArray, NUMBER_ENTRIES);

	for(i=0; i<NUMBER_ENTRIES; i++)
	{
		printf("The integer array value %x has %d bits", integerArray[i], countBits(integerArray[i]));
		printf("\n");
	}

	floatQuickSort(floatArray, 0, NUMBER_ENTRIES-1);
	integerQuickSort(integerArray, 0, NUMBER_ENTRIES-1);
	printf("The sorted integers are:\n");
	showIntegers(integerArray, NUMBER_ENTRIES);
	printf("The sorted floats are:\n");
	showFloats(floatArray, NUMBER_ENTRIES);
	printf("The median value of the integers array is:%.2f\n", medianInteger(integerArray, NUMBER_ENTRIES-1));
	printf("The median value of the float array is:%.2f\n", medianFloat(floatArray, NUMBER_ENTRIES-1));

	for(i=0; i<NUMBER_ENTRIES; i++)
	{
		printf("The unsigned short value %#x :", (unsigned short)integerArray[i]);
		printf("\n");
		binaryString(binaryStr, 16, integerArray[i]);	
		printf("  ORIG : %s", binaryStr);
		unsigned short temp = reverseBits((unsigned short) integerArray[i]);
		binaryString(binaryStr, 16, (int) temp);	
		printf("  REVR : %s", binaryStr);
		printf("\n");
	}
	
	showCDF(integerArray, NUMBER_ENTRIES);
	// Return successfully
	return( 1 );
}
