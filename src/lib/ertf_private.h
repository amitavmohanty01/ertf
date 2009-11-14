#ifndef ERTF_PRIVATE_H_
#define ERTF_PRIVATE_H_


#include <Eina.h>


typedef struct Ertf_Summary Ertf_Summary;

struct Ertf_Document
{
  char                 *filename;
  FILE                 *stream;
  char                 *markup;
  Ertf_Summary         *summary;
  int                   markup_position;
  int                   version;
  int                   bracecount;
  Ertf_Document_Charset charset;
};


extern unsigned char _ertf_default_color_r;
extern unsigned char _ertf_default_color_g;
extern unsigned char _ertf_default_color_b;

extern int _ertf_paper_width;
extern int _ertf_paper_height;
extern int _ertf_margin_left;
extern int _ertf_margin_right;
extern int _ertf_margin_top;
extern int _ertf_margin_bottom;

extern int _ertf_default_font;
extern int _twip_scale_factor;

extern Eina_Array *color_table;
extern Eina_Array *font_table;
extern Eina_Array *stylesheet_table;
extern int         ertf_markup_position;
extern char       *markup;


#endif /* ERTF_PRIVATE_H_ */
