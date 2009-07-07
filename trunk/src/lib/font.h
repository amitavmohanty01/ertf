#ifndef FONT_H
#define FONT_H
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
typedef struct font_node{
  int number;
  char family[7];
  // todo: a font family name may be replaced by an eina string share
  char name[20];  
}FONT;
Eina_Array *font_table;
int ertf_font_table(FILE *);
#endif
