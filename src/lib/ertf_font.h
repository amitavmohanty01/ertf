#ifndef ERTF_FONT_H_
#define ERTF_FONT_H_


#include <stdio.h>

/*
 * Supported font families:
 * 1. Roman
 * 2. swiss
 * 3. modern
 * 4. script
 * 5. decor
 * 6. tech
 */

#define NAME_SET 1
#define FAMILY_SET 2
#define CHARSET_SET 4

typedef struct Ertf_Font_Node{
  char name[200];
  char family[7];
  char status;
  int number;
  int charset;
  // todo: a font family name may be replaced by an eina string share
} Ertf_Font_Node;

int ertf_font_table(FILE *);


#endif /* ERTF_FONT_H_ */
