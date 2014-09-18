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
//  Created       : between the time this was assigned and the due date

/* Variable declarations */

/* Loops through the array of floats, printing each one*/
void showFloats( float* floatArray, int length )
{
	int i = 0;
	for( i = 0; i<length; i++ )
	{
		// Included the .2 so only 2 decimal places were printed, and 
		// the tab because that's what the sample output looked like 
		printf("\t%.2f", floatArray[i] );			
	}
	printf("\n");
}

/* Loops through the array of ints printing each one*/
void showIntegers( int* intArray, int length )
{
	int i = 0;
	for( i = 0; i<length; i++ )
        {
		// Again, similar deal to what's in show floats
                printf("\t%d", intArray[i] );
        }
	printf("\n");
}

/* Finds the median. If the length is an even value, divide the length by two, and 
 * find the average of the middle value and the value to its right*/
float medianFloat( float* floatArray, int length )
{
	// Arrays need to be sorted here or prior to being sent to this function

	// Confirm that length values passed to this method starts at zero
	if((length+1)%2 == 0)
	{
		// Value at the left of the middle
		float left =  floatArray[(length)/2];
		// Value at the right
		float right = floatArray[((length)/2)+1];

		// return the average of the two
		return ((left+right)/2);
	}

	return floatArray[(length+1)/2];
}

/* Finds the median. If the length is an even value, divide the length by two, and 
 * find the average of the middle value and the value to its right*/
float medianInteger( int* intArray, int length )
{
	// Arrays need to be sorted here or prior to being sent to this function

        // Confirm that length values passed to this method starts at zero
        if((length+1)%2 == 0)
        {
                int left =  intArray[(length)/2];
                int right = intArray[((length)/2)+1];

                return ((left+right)/2);
        }

        return intArray[(length+1)/2];

}

/* Takes an arbitrary integer and finds the number of '1' bits that exist in its binary representation*/
int countBits( int integer )
{
	int numberOfBits = 0;
	// Ensure that the integer is non-negative
	integer = abs(integer);
	
	// Loop while the integer is greater than or equal to 1
	while(integer>=1)
	{
		// If the bitwise and of the value with 1 is 1, we increment the bit count
		if(integer&1)
			numberOfBits+=1;
		// Values are Big Endian in c, so we shift right by one each iteration
		integer>>=1;	
	}
	
	return numberOfBits;
}
// Converts an integer to a string of binary values
void binaryString( char* value, int length, int toConvertToBinary )
{
	// Debug code
	//printf("\n");
	//printf("value of length: %d", length);
	//printf("\n");

        // Ensure that the integer is non-negative
        int numb = abs( toConvertToBinary );
 
        int i;
	for(i=length-2; i>=0; i--)
	{
		// Shift the bits by the iteration count so the top most
		// bits are in the ith position of the array
		// Takes into account the fact that we're dealing with big endian binary
		int temp = numb >> i;
		if(temp & 1)
		{
			*(value +length-i-2) = '1';
		}
		else
			*(value +length-i-2) = '0';
			
	}
	// Append a null character to make it null terminating
	*(value + length-1) = '\0';

}

/* Reverses the bits of some short value*/
unsigned short reverseBits( unsigned short value )
{ 
	unsigned short returnNum = 0;
	int i = 0;
	unsigned short temp;
	//printf("\n");
	//printf("values of i:");
	for (i = 0; i < 16; i++)
	{	
		// Debug code
	//	printf("%d", i);
	//	// Assigns temp the value of value's ith bit. Which is always just 1 or 0
		temp = (value & (1 << i));
		// If temp isn't 0
		if(temp)
			// Add the value of 15-ith most bit to the return value
			returnNum += (1<<(15 - i));
	}
	//printf("\n");
	return returnNum;
}	

// Swaps two elements in an array of integers
// this could probably be made into a template function
// Used in conjunction with quicksort
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
	
	// Swaps the pivot out for the right most index
	integerSwap(toPart, pivotIndex, right);

	// Loop from the left value (usually left most element to pivotIndex-1)
	for(i=left; i<right; i++)
	{
		// If ith element is less than the pivot, swap that shit with the storedIndex
		// storedIndex ensures that we always move values below where the pivot's final 
		// position will be
		if(toPart[i]<pivot)
		{
			integerSwap(toPart, i, storeIndex);
			storeIndex += 1;
		}
	}

	// Put the pivot in its place
	integerSwap(toPart, storeIndex, right);
	return storeIndex;
}

/* Recursive quicksort */
void integerQuickSort( int* intArray, int left, int right )
{
	// Ensures that we aren't sorting an array of length 1
	if(left<right)
	{
		int p = integerPart(intArray, left, right);
		integerQuickSort(intArray, left, p-1);
		integerQuickSort(intArray, p+1, right);
	}

}

/* Used in conjunction with showCDF
 * basically just takes the cdf array, and int array and finds if a value in the int array
 * corresponds to a value in the cdf array. if not it returns the cdf of the value just below
 * the next greatest cdf's value*/
float getCDF( float* cdfArray, int* intArray, int value, int length )
{
	int i = 0;
	int index = -1;
	int indexOfGreater = -1;
	for(i=0; i<=length; i++)
	{
		if(intArray[i] == value)
		{
			index = i;
			break;
		}
		if(intArray[i]>value && indexOfGreater == -1)
		{
			indexOfGreater = i;
		}
	}
	if(index != -1)
		return cdfArray[index];
	else 
		return cdfArray[indexOfGreater-1];
}

/* Uses getCDF
 * computes the cdf of the values in the array and then outputs a graph of them to the console*/
void showCDF( int* array, int length )
{
	int i = 0;
	float cdf[length];
	int a = array[0];
	int b = array[length-1];

	for(i=0;i<length; i++)
	{
		float cdfValue = (float)(i+1)/length;
		//printf("%f", cdfValue);
		//printf("\n");
		cdf[i] = cdfValue;//roundf((float) ((i+1)/length))*100;
	}
	/* Prints the graph itself
	 * iterates from 100 to 0 printing shit*/
	for(i=100; i>=0; i-=5)
	{
		printf("%3d +", i);
		int j = 0;

		/* This loop determines whether or not a space or a 
		 * star is printed depending on whether or not the 
		 * cdf we found for the given value is >= the current value
		 * of i*/
		for(j=0;j<=b-a;j++)
		{
			float cdfToPrint = getCDF(cdf, array, j+a, length);
			int cdfToInt = (int) (cdfToPrint*100);
			if( cdfToInt >= i)
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
	/* From here on out it's just printing the bottom part of the 
	 * graph that is displayed. The last 3 lines that you normally see*/
	printf("    +");
	for(i=0; i<=b-a; i++)
	{
		printf("-");
	}
	printf("\n");
	
	printf("    +");
	for(i=a; i<=b; i++)
	{
		if(i<10)
			printf(" ");
		else
			printf("%d", i/10);
	}
	printf("\n");

	printf("    +");
	for(i=a; i<=b; i++)
	{
		printf("%d", i%10);
	}
	printf("\n");
}

/* See comments for integer quicksort*/
void floatSwap(float* array, int index1, int index2)
{
	float value = array[index1];
	array[index1] = array[index2];
	array[index2] = value;
}

/* Only difference here between this and integer quicksort are the variables
 * and the fact that we sort from greatest to least
 * See comments for integer quicksort*/
int floatPart(float* toPart, int left, int right)
{
	int i = 0;
	int storeIndex = left;
	int pivotIndex = (left+right)/2;
	float pivot = toPart[pivotIndex];
	floatSwap(toPart, pivotIndex, right);

	// might be to right-1
	for(i=left; i<right; i++)
	{
		// This is the only difference.
		if(toPart[i]>pivot)
		{
			floatSwap(toPart, i, storeIndex);
			storeIndex += 1;
		}
	}

	floatSwap(toPart, storeIndex, right);
	return storeIndex;
}

/* See comments for integer quicksort*/
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
