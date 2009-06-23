#ifndef COLOUR_H
#define COLOUR_H
#include <stdio.h>
#include <eina_array.h>
typedef struct colour{
  char r,g,b;
}COLOUR;
int ertf_colortbl(FILE *);
Eina_Array *colour_table;
#endif
