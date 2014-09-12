#ifndef A2SUPPORT_INCLUDED
#define A2SUPPORT_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : a2support.h
//  Description   : This is the header file for the functions for assignment
//                  2 of the CMPSC311 course.  Students are required to define
//                  the following functions and implement them in another
//                  file, a2support.c.
//
//  Author        : ???
//  Created       : ???

// Functions to define

void showFloats( float* floatArray, int length );
void showIntegers( int* intArray, int length );
float medianFloat( float* floatArray, int length );
float medianInteger( int* intArray, int length );
int countBits( int integer );
void binaryString( char* value, int length, int toConvertToBinary );
unsigned short reverseBits( unsigned short int value );
void floatQuickSort( float* floatArray, int left, int right );
void integerQuickSort( int* intArray, int left, int right );
void showCDF( int* intArray, int lengt );

#endif
