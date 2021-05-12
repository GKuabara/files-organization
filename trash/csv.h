#ifndef _CSV_H
#define _CSV_H

struct _delim {
    int amnt_delimitators;
    char **delimitators;
};

#define default_term_ (int[]){'\n', EOF}


char **linear_stream_get_tokens(char* stream, const struct _delim d);




#endif

