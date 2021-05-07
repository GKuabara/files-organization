#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <assert.h>

#include "utils.h"
#include "parser.h"

static int regex_compiler(regex_t *, const string pattern, const int flags);
static string regex_matcher(regex_t *, string line, regmatch_t *, int *start);

/* Compiles a `reg` with a specified `pattern` and `flag`. */
static int regex_compiler(regex_t *reg, const string pattern, const int flags) {
    if (reg == NULL || pattern == NULL) { // If invalid arguments are passed
        return ERROR;
    }

    if (regcomp(reg, pattern, flags) != 0) {
        return ERROR;
    }

    return SUCCESS;
}

/* Copies and returns blocks in a `line` that match the `reg` pattern. */ 
static string regex_matcher(regex_t *reg, string line, regmatch_t *match, int *start) { 
    if (regexec(reg, line, 1, match, REG_NOTBOL) != REG_NOERROR) { // Copies the last block
        char *matching = my_strndup(line, strlen(line));
        *start = -1;
        return matching;
    }

    *start += match->rm_eo;
    return my_strndup(line, match->rm_eo - 1);
}
    
/* Parses a `line` given a `reader` instance. */
void parser_read(Reader reader, string line) {
    assert(reader != NULL);

    regmatch_t match;
    int i = 0;
    int start = 0;
    do { // Copies values that match
        reader->values = realloc(reader->values, sizeof(char *) * (i + 1));
        reader->values[i] = regex_matcher(&reader->pattern, line + start, &match, &start);
        i++;
    } while (start != -1);

    reader->amnt_values = i;
}

/* Initializes a `Reader` instance given a parsing pattern. */
Reader parser_reader_init(string pattern) {
    Reader reader = malloc(sizeof(struct reader));
    assert(reader != NULL);

    assert(regex_compiler(&reader->pattern, pattern, REG_EXTENDED) != ERROR);
    reader->amnt_values = 0;
    reader->values = NULL;

    return reader;
}

/* Cleans the `values` in a `Reader`. */
void parser_clean_reader(Reader reader) {
    assert(reader != NULL);
    
    if (reader->values != NULL) {
        for (int i = 0; i < reader->amnt_values; ++i) {
            if (reader->values[i] != NULL) {
                free(reader->values[i]);
            }
            reader->values[i] = NULL;
        }
        free(reader->values);
    }

    reader->values = NULL;
    reader->amnt_values = 0;
}

/* Deletes a `Reader` instance. */
void parser_reader_delete(Reader *reader) {
    assert(*reader != NULL);

    regfree(&(*reader)->pattern);  
    parser_clean_reader(*reader);
    
    free(*reader);
    *reader = NULL;
}

/* Reads the header of a `stream` given its parsing `pattern`. */
Header parser_header_init(FILE *stream, string pattern) {
    Header header;
    Reader tmp_reader = parser_reader_init(pattern);

    assert((header = malloc(sizeof(struct header))) != NULL);    

    fseek(stream, 0, SEEK_SET); // Make sure the pointer is at the beginning of the stream
    string header_str = readline(stream);

    parser_read(tmp_reader, header_str);
    free(header_str);
    
    header->fields = tmp_reader->values;
    header->amnt_fields = tmp_reader->amnt_values;
    
    tmp_reader->values = NULL;
    parser_reader_delete(&tmp_reader);
    
    return header;
}

/* Deletes an `Header` instance. */
void parser_header_delete(Header *header) {
    assert( *header != NULL);
    
    for (int i = 0; i < (*header)->amnt_fields; ++i) {
        free((*header)->fields[i]);
        (*header)->fields[i] = NULL;
    } 
    
    free((*header)->fields);
    (*header)->fields = NULL;

    free(*header);
    *header = NULL;
}