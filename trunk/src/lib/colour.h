#ifndef COLOUR_H
#define COLOUR_H
#include <stdio.h>
#include <eina_array.h>
typedef struct colour{
  unsigned char r,g,b;
  char string[9];
}COLOUR;
int ertf_colour_table(FILE *);
Eina_Array *colour_table;
#endif
