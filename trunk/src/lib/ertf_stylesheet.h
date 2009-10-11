#ifndef ERTF_STYLESHEET_H_
#define ERTF_STYLESHEET_H_


#include <stdio.h>

#define STYLE_NAME_SET 1
#define STYLE_FONT_SET 2
#define STYLE_FONT_SIZE_SET 4
#define STYLE_FOREGROUND_SET 8
#define STYLE_BACKGROUND_SET 16

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

char *ertf_textblock_style_get();


#endif /* ERTF_STYLESHEET_H_ */
