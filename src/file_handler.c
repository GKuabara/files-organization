#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "file_handler.h"

static int _file_error(FILE *stream, string format, ...);

/*
    Error handling. 
*/
int _file_error(FILE *stream, string format, ...) {
    int ferr = stream ? ferror(stream) : -1;
    if (!ferr) return ferr; 
    
    va_list args;
    va_start(args, format);
    printf(format, args);
   
    va_end(args); 
    if (stream) clearerr(stream);

    return ferr;
}

/*
    `fopen()` + error handling
*/
FILE* file_open(const char *filename, const char *modes) {
    FILE *stream = fopen(filename, modes);

    if (!stream) _file_error(stream, ERROR_MESSAGE); 

    return stream;
}   

/*
    `fclose()` `n` given files.
*/
void files_close(int n, ...) {
    va_list fps;
    va_start(fps, n);

    while (n--) {
        FILE *fp = va_arg(fps, FILE *);
        if (fp) fclose(fp);
    }
    
    va_end(fps);
}   

#ifndef IGNORE_WANDR_ERRORS

/*
    `fwrite()` + error handling.
*/
void file_write(void *prt, size_t size, size_t n, FILE *stream) {
    if (fwrite(prt, size, n, stream) != n) 
        exit(_file_error(stream, ERROR_MESSAGE));
}

/*
    `fread()` + error handling.
*/
void file_read(void *prt, size_t size, size_t n, FILE *stream) {
    if (fread(prt, size, n, stream) != n) 
        exit(_file_error(stream, ERROR_MESSAGE));
}
#endif

#ifdef IGNORE_WANDR_ERRORS
/*
    `fwrite()` + error handling.
*/
int file_write(void *prt, size_t size, size_t n, FILE *stream) {
    if (fwrite(prt, size, n, stream) != n) 
        return _file_error(stream, ERROR_MESSAGE);
}

/*
    `fread()` + error handling.
*/
int file_read(void *prt, size_t size, size_t n, FILE *stream) {
    if (fread(prt, size, n, stream) != n) 
        return _file_error(stream, ERROR_MESSAGE);
}
#endif