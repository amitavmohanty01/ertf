#ifndef ERTF_FONT_H_
#define ERTF_FONT_H_


#include <stdio.h>
#include <eina_array.h>

/*
 * Supported font families:
 * 1. Roman
 * 2. swiss
 * 3. modern
 * 4. script
 * 5. decor
 * 6. tech
 */

typedef struct Ertf_Font_Node{
  char name[20];
  char family[7];
  int number;
  // todo: a font family name may be replaced by an eina string share
} Ertf_Font_Node;

Eina_Array *font_table;

int ertf_font_table(FILE *);


#endif /* ERTF_FONT_H_ */
