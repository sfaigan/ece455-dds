/*
 * tiny_printf.h
 *
 *  Created on: Mar 30, 2022
 *      Author: sfaigan
 */

#ifndef TINY_PRINTF_H_
#define TINY_PRINTF_H_

#include <stdarg.h>
#include <stdio.h>

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given character string according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int sprintf(char *buf, const char *fmt, ...);

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            given file stream according to the format parameter.
**  Returns:  Number of bytes written
**===========================================================================
*/
int fprintf(FILE * stream, const char *fmt, ...);

/**
**===========================================================================
**  Abstract: Loads data from the given locations and writes them to the
**            standard output according to the format parameter.
**  Returns:  Number of bytes written
**
**===========================================================================
*/
int tiny_printf(const char *fmt, ...);

#endif /* TINY_PRINTF_H_ */
