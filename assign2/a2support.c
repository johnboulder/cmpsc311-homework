#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "a2support.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//
//  File          : a2support.c
//  Description   : This is the implimentation file for the functions for assignment
//                  2 of the CMPSC311 course.
//
//  Author        : John Walter Stockwell
//  Created       : ???

/* Variable declarations */


void showFloats( float* floatArray, int length )
{
	int i = 0;
	for( i = 0; i<length; i++ )
	{
		printf("	%f", floatArray[i] );			
	}
}
void showIntegers( int* intArray, int length )
{
	int i = 0;
	for( i = 0; i<length; i++ )
        {
                printf("        %d", intArray[i] );
        }
}
float medianFloat( float* floatArray, int length )
{
	// Arrays need to be sorted here or prior to being sent to this function

	// Confirm that length values passed to this method starts at zero
	if((length+1)%2 == 0)
	{
		float left =  floatArray[(length+1)/2];
		float right = floatArray[((length+1)/2)+1];

		return ((left+right)/2);
	}

	return floatArray[(length+1)/2];
}
float medianInteger( int* intArray, int length )
{
	// Arrays need to be sorted here or prior to being sent to this function

        // Confirm that length values passed to this method starts at zero
        if((length+1)%2 == 0)
        {
                float left =  intArray[(length+1)/2];
                float right = intArray[((length+1)/2)+1];

                return ((left+right)/2);
        }

        return intArray[(length+1)/2];

}

// Question, does this take a normal integer and convert it to binary, and then count its bits?
// Or does it take a binary array?

// We're going to assume we're taking a integer value and converting it to binary
int countBits( int integer )
{
	int numberOfBits = 0;
	// Ensure that the integer is non-negative
	integer = abs(integer);
	
	// Loop while the integer is greater than or equal to 1
	while(integer>=1)
	{
		if(integer%2 == 1)
			numberOfBits+=1;

		integer/=2;	
	}
	
	return numberOfBits;
}
void binaryString( char* value, int length, int toConvertToBinary )
{
        // Ensure that the integer is non-negative
        int integer = abs( toConvertToBinary );

        // Loop while the integer is greater than or equal to 1
        while( integer>=1 )
        {
                if( integer%2 == 1 )
			*value = '1';
		else if( integer%2 == 0)
			*value = '0';
                integer/=2;
		value+=1;
        }
	
	*value = '\n';
}
// Ask about this. We're reversing bits, but I guess, does he want us to be converting to binary as well
// or did he intend for this to be an array of ints?
unsigned short reverseBits( unsigned short value )
{
	static const int n = 16; // Minimum bits in a short
	double bitArray[n];
	double* ptr = &bitArray[0];
	int i = 0;
        while( value>=1 ) 
        {
		*ptr = value%2;
                value/=2;
		ptr++;
        }
	double returnValueDble = 0;
	double j = 0;
	for( i = 0; i < n; i++)
	{
		returnValueDble+=pow(bitArray[i] * 2, j);
		j++;
	} 
	return (unsigned short) returnValueDble;
}

void floatQuickSort( float* floatArray, int left, int right )
{

}
void integerQuickSort( int* intArray, int left, int right )
{

}
void showCDF( int* intArray, int length)
{

}
 
