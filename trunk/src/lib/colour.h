#ifndef ERTF_COLOR_H_
#define ERTF_COLOR_H_


#include <stdio.h>

#include <eina_array.h>

typedef struct Ertf_Color{
  char string[9];
  unsigned char r;
  unsigned char g;
  unsigned char b;
} Ertf_Color;

int ertf_color_table(FILE *);

Eina_Array *color_table;


#endif /* ERTF_COLOR_H_ */
