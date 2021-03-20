/**
 * @file stdlib.h
 *
 * $Id: stdlib.h 2051 2009-08-27 20:55:09Z akoehler $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#define RAND_MAX 2147483646

int abs(int);
long labs(long);
int atoi(char *);
long atol(char *);
void bzero(void *, int);
void qsort(char *, unsigned int, int, int (*)(void));
int rand(void);
void srand(unsigned int);
void *malloc(unsigned int nbytes);
void free(void *pmem);
