#ifndef ERTF_STYLESHEET_H_
#define ERTF_STYLESHEET_H_


#include <eina_array.h>
#include <stdio.h>

Eina_Array *stylesheet_table;

typedef struct Ertf_Stylesheet
{
  char name[20];
  // assumption: style names won't be longer than 20 chars
  // todo: find exceptions to the assumption and frequency of their occurrence

  int set;
  // used to check multiple occurence of the same tag and also during generating
  // the style string

  unsigned int style_number;
  // since the RTF writers are not bound to write styles in order, we need to store style numbers.

  unsigned int font_number;
  // indicates entry number in font table

  unsigned int font_size;
  // todo: check what font sizes shall be invalid, i.e. the upper bound

  unsigned int foreground_colour;// indicates entry number in colour table
  unsigned int background_colour;// indicates entry number in colour table
} Ertf_Stylesheet;

int ertf_stylesheet_parse(FILE *);


#endif /* ERTF_STYLESHEET_H_ */
