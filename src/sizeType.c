// This program prints the size of different variable types in bytes

#include <stdio.h>
#include <stdlib.h>
#include <NIDAQmx.h> // for NIDAQmx

int main()
{
    printf("Size of char: %ld byte(s)\n", sizeof(char));
    printf("Size of unsigned char: %ld byte(s)\n", sizeof(unsigned char));
    printf("Size of signed char: %ld byte(s)\n", sizeof(signed char));
    printf("Size of int: %ld byte(s)\n", sizeof(int));
    printf("Size of unsigned int: %ld byte(s)\n", sizeof(unsigned int));
    printf("Size of signed int: %ld byte(s)\n", sizeof(signed int));
    printf("Size of short: %ld byte(s)\n", sizeof(short));
    printf("Size of unsigned short: %ld byte(s)\n", sizeof(unsigned short));
    printf("Size of signed short: %ld byte(s)\n", sizeof(signed short));
    printf("Size of long: %ld byte(s)\n", sizeof(long));
    printf("Size of unsigned long: %ld byte(s)\n", sizeof(unsigned long));
    printf("Size of signed long: %ld byte(s)\n", sizeof(signed long));
    printf("Size of long long: %ld byte(s)\n", sizeof(long long));
    printf("Size of unsigned long long: %ld byte(s)\n", sizeof(unsigned long long));
    printf("Size of signed long long: %ld byte(s)\n", sizeof(signed long long));
    printf("Size of float: %ld byte(s)\n", sizeof(float));
    printf("Size of double: %ld byte(s)\n", sizeof(double));
    printf("Size of long double: %ld byte(s)\n", sizeof(long double));
    printf("Size of size_t: %ld byte(s)\n", sizeof(size_t));
    printf("Size of int8: %ld byte(s)\n", sizeof(int8));
    printf("Size of int16: %ld byte(s)\n", sizeof(int16));
    printf("Size of int32: %ld byte(s)\n", sizeof(int32));
    printf("Size of int64: %ld byte(s)\n", sizeof(int64));
    printf("Size of float32: %ld byte(s)\n", sizeof(float32));
    printf("Size of float64: %ld byte(s)\n", sizeof(float64));


    return 0;
}