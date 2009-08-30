#ifndef ERTF_COLOR_H_
#define ERTF_COLOR_H_


#include <stdio.h>

typedef struct Ertf_Color{
  char string[9];
  unsigned char r;
  unsigned char g;
  unsigned char b;
} Ertf_Color;

int ertf_color_table(FILE *);


#endif /* ERTF_COLOR_H_ */
