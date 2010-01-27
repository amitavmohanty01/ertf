#ifndef ERTF_PRIVATE_H_
#define ERTF_PRIVATE_H_


#include <Eina.h>


struct Ertf_Summary{
  // todo: check if there should be a limitation to the length of the strings
  char *author;
  char *title;
  char *subject;
  char *operator;
  char *keywords;
  char *comment;
  int version;
  char *doccomm;
  int internal_version;
  int pages;
  long int words;
  long int chars;
  int internal_ID;
  // todo: add time variables
};

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

// for debugging
extern int _line;

 
extern int _ertf_log_dom;
#ifdef ERTF_DEFAULT_LOG_COLOR
# undef ERTF_DEFAULT_LOG_COLOR
#endif
#define ERTF_DEFAULT_LOG_COLOR EINA_COLOR_BLUE
#ifdef ERR 
# undef ERR 
#endif 
#define ERR(...) EINA_LOG_DOM_ERR(_ertf_log_dom, __VA_ARGS__) 
#ifdef INFO
# undef INFO 
#endif 
#define INFO(...) EINA_LOG_DOM_INFO(_ertf_log_dom, __VA_ARGS__) 
#ifdef WARN
# undef WARN
#endif 
#define WARN(...) EINA_LOG_DOM_WARN(_ertf_log_dom, __VA_ARGS__)
#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_ertf_log_dom, __VA_ARGS__)


#endif /* ERTF_PRIVATE_H_ */
