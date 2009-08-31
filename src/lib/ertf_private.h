#ifndef ERTF_PRIVATE_H_
#define ERTF_PRIVATE_H_


#include <eina_array.h>


extern unsigned char _ertf_default_color_r;
extern unsigned char _ertf_default_color_g;
extern unsigned char _ertf_default_color_b;

extern Eina_Array *color_table;
extern Eina_Array *font_table;
extern Eina_Array *stylesheet_table;
extern int         ertf_markup_position;
extern char       *markup;


#endif /* ERTF_PRIVATE_H_ */
