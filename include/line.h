#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"

#define l_code_desc_t char[15]
#define l_card_desc_t char[13]
#define l_name_desc_t char[13]
#define l_color_desc_t char [24]

#define L_HEADER_SIZE 82
#define L_CONST_REG_SIZE 13

/* Card options */
#define S 'S' /* no payment */
#define N 'N' /* also accepts money */
#define F 'F' /* only card on weekends */

enum {
    CODE,
    CARD,
    NAME,
    COLOR
};

void l_insert_datareg(FILE *bin, string *tokens);
void l_header_init(struct _g_files *files);

#endif
