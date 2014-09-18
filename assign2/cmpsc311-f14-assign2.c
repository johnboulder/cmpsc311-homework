////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311-f14-assign2.c
//  Description   : This is the main code file for the CMPSC311 assignment 2.
//                  see class assignment for details on what needs to be added.
//
//  Author        : John Walter Stockwell
//  Created       : 9/16/2014
//
#include <stdio.h>
#include "a2support.h"
#include <math.h>
#define NUMBER_ENTRIES 20
#define BINARY_INT_STRING_LENGTH 17

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
	char binaryStr[BINARY_INT_STRING_LENGTH];
	// Read the integer values
	for ( i=0; i<NUMBER_ENTRIES; i++ ) {
	    scanf( "%d", &integerArray[i] );
	}
	
	float floatArray[NUMBER_ENTRIES];
	// Find the cosine and sine of the values in the int array
	for( i=0; i<NUMBER_ENTRIES; i++ )
	{
		if( i%2 == 0 )
			floatArray[i] = roundf(cosf((float) integerArray[i])*100) /100;
		else
			floatArray[i] = roundf(sinf((float) integerArray[i])*100) /100;
			
	}
	showIntegers(integerArray, NUMBER_ENTRIES);
	showFloats(floatArray, NUMBER_ENTRIES);
	
	// Output the values of the hex value of the numbers in the in array, and the number of bits
	for(i=0; i<NUMBER_ENTRIES; i++)
	{
		printf("The integer array value %x has %d bits", integerArray[i], countBits(integerArray[i]));
		printf("\n");
	}

	// Quicksort stuff
	floatQuickSort(floatArray, 0, NUMBER_ENTRIES-1);
	integerQuickSort(integerArray, 0, NUMBER_ENTRIES-1);
	printf("The sorted integers are:\n");
	showIntegers(integerArray, NUMBER_ENTRIES);
	printf("The sorted floats are:\n");
	showFloats(floatArray, NUMBER_ENTRIES);
	// Output stuff with formatting
	printf("The median value of the integers array is:%.2f\n", medianInteger(integerArray, NUMBER_ENTRIES-1));
	printf("The median value of the float array is:%.2f\n", medianFloat(floatArray, NUMBER_ENTRIES-1));
	
	// Loops through the array printing the binary value of each integer element, and reverses that value
	for(i=0; i<NUMBER_ENTRIES; i++)
	{
		printf("The unsigned short value %#x :", (unsigned short)integerArray[i]);
		printf("\n");
		binaryString(binaryStr, BINARY_INT_STRING_LENGTH, integerArray[i]);	
		printf("  ORIG : %s", binaryStr);
		printf("\n");
		unsigned short temp = reverseBits((unsigned short) integerArray[i]);
		binaryString(binaryStr, BINARY_INT_STRING_LENGTH, (int) temp);	
		printf("  REVR : %s", binaryStr);
		printf("\n");
	}

	// Self explanatory
	showCDF(integerArray, NUMBER_ENTRIES);
	// Return successfully
	return( 1 );
}
