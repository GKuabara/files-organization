#ifndef _LINE_H
#define _LINE_H

#include "stream.h"
#include "global.h"

#define l_code_desc_t char[15]
#define l_card_desc_t char[13]
#define l_name_desc_t char[13]
#define l_color_desc_t char [24]

/* Card options */
#define S 'S' /* no payment */
#define N 'N' /* also accepts money */
#define F 'F' /* only card on weekends */

#define L_HEADER_SIZE 82

typedef struct{
    int code;
    char card;
    int name_size;
    char *line_name;
    int color_size;
    char *color;
} line;

void line_select(FILE *fp, int last_byte);

#endif
