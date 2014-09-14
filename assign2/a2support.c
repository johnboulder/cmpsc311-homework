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

// Swaps two elements in an array of integers
// this could probably be made into a template function
void integerSwap(int* array, int index1, int index2)
{
	int value = array[index1];
	array[index1] = array[index2];
	array[index2] = value;
}

// Partitions an integer array into two semi-sorted parts
// Used by integerQuicksort
int integerPart(int* toPart, int left, int right)
{
	int i = 0;
	int storeIndex = left;
	int pivotIndex = (left+right)/2;
	int pivot = toPart[pivotIndex];
	integerSwap(toPart, pivotIndex, right);

	//might be to right-1
	for(i=left; i<right; i++)
	{
		if(toPart[i]<pivot)
		{
			integerSwap(toPart, i, storeIndex);
			storeIndex += 1;
		}
	}

	integerSwap(toPart, storeIndex, right);
	return storeIndex;
}

void integerQuickSort( int* intArray, int left, int right )
{
	if(left<right)
	{
		int p = integerPart(intArray, left, right);
		integerQuickSort(intArray, left, p-1);
		integerQuickSort(intArray, p+1, right);
	}

}

void showCDF( int* array, int length )
{
	int a = array[0];
	int b = array[length-1];
	// Probability that some X, a<X<=b, can be described as X<=x. The probability that some random X will be less than or equal to an x in the set of values.

	// Iterate from the top of the array to the bottom
}

void floatSwap(float* array, int index1, int index2)
{
	float value = array[index1];
	array[index1] = array[index2];
	array[index2] = value;
}

int floatPart(float* toPart, int left, int right)
{
	int i = 0;
	int storeIndex = left;
	int pivotIndex = (left+right)/2;
	float pivot = toPart[pivotIndex];
	floatSwap(toPart, pivotIndex, right);

	//might be to right-1
	for(i=left; i<right; i++)
	{
		if(toPart[i]<pivot)
		{
			floatSwap(toPart, i, storeIndex);
			storeIndex += 1;
		}
	}

	floatSwap(toPart, storeIndex, right);
	return storeIndex;
}

void floatQuickSort( float* floatArray, int left, int right )
{
	if(left<right)
	{
		int p = floatPart(floatArray, left, right);
		floatQuickSort(floatArray, left, p-1);
		floatQuickSort(floatArray, p+1, right);
	}
}

// Not fully implemented. Saving for later in case in the future if my quicksorts need to be improved
int choosePivot(float* array, int left, int right)
{
	int i = 0;
	float* greatest = &array[0];
	float* least = &array[0];
	float* median = &array[0];
	float fleft = array[left];
	float fright = array[right];
	float middle = array[(left+right)/2];
	float medianArray[3] = {fleft, middle, fright};
	for(i = 0; i<3; i++)
	{
		if(medianArray[i] > *greatest)
			greatest = &medianArray[i];
		if(medianArray[i] < *least)
			least = &medianArray[i];
	}
	if(*median == middle)
		return 1; 
	return 0;
}
