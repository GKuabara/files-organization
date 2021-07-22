#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include <stdio.h>

#include "stream.h"

#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE "Falha no processamento do arquivo.\n"
#endif

typedef struct { 
    FILE *bin;
    FILE *csv;
} files_t;

/*
    `fopen()` + error handling
*/
FILE* file_open(const char *filename, const char *modes);

/*
    `fclose()` `n` given files.
*/
void files_close(int n, ...);

#ifndef IGNORE_WANDR_ERRORS // exits the program
/*
    `fwrite()` + error handling.
*/
void file_write(void *prt, size_t size,  size_t n, FILE *stream);
/*
    `fread()` + error handling.
*/
void file_read(void *prt, size_t size, size_t n, FILE *stream);
#endif

#ifdef IGNORE_WANDR_ERRORS // returns error value
/*
    `fwrite()` + error handling.
*/
int file_write(void *prt, size_t size, size_t n, FILE *stream);
/*
    `fread()` + error handling.
*/
int file_read(void *prt, size_t size, size_t n, FILE *stream);
#endif

#endif