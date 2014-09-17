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
		printf("	%.2f", floatArray[i] );			
	}
	printf("\n");
}

void showIntegers( int* intArray, int length )
{
	int i = 0;
	for( i = 0; i<length; i++ )
        {
                printf("        %d", intArray[i] );
        }
	printf("\n");
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

	//value[length] = '\0';
        // 
        int i = 0;
	while(i<16)
	{
		*value = 0;
		value+=1;
		i++;
	}
	value[length] = '\0';
	while(i>-1)
	{
		if(integer & 0x1)
			*value = '1';
		integer>>=1;
		i--;
		value--;
	}
}

unsigned short reverseBits( unsigned short value )
{ 
	unsigned short returnNum = 0;
	int i = 0;
	unsigned short temp;
	for (i = 0; i <= 16; i++)
	{
		temp = (value & (1 << i));
		if(temp)
			returnNum |= (1<<(15 - i));
	}
	return returnNum;
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

	for(i=100; i>=0; i-=5)
	{
		printf("%3d +", i);
		int j = 0;
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
