#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include <Eina.h>

#include "Ertf.h"
#include "ertf_main.h"
#include "ertf_private.h"

unsigned char _ertf_default_color_r = 0;
unsigned char _ertf_default_color_g = 0;
unsigned char _ertf_default_color_b = 0;

/* The following values are default values in twips. */
int _ertf_paper_width   = 12240;
int _ertf_paper_height  = 15840;
int _ertf_margin_left   = 1800;
int _ertf_margin_right  = 1800;
int _ertf_margin_top    = 1440;
int _ertf_margin_bottom = 1440;

int _ertf_default_font = 0;
/* Usually, this is the screen dpi */
int _twip_scale_factor = 86;

int _ertf_log_dom = -1;

// for debugging
int _line = 1;

// string buffers
Eina_Strbuf *markup_buf;
Eina_Strbuf *style_buf;

static int _ertf_initcount = 0;

static void _ertf_cleanup();

int
ertf_init(void)
{
  if (_ertf_initcount)
    goto finish_init;

  if (!eina_init())
  {
    fprintf(stderr, "Could not initialize eina");
    return 0;
  }

  _ertf_log_dom = eina_log_domain_register("Ertf", ERTF_DEFAULT_LOG_COLOR);
  if (_ertf_log_dom < 0)
  {
    EINA_LOG_ERR("Ertf can't create a general log domain.\n");
  }

  markup_buf = eina_strbuf_new();
  style_buf = eina_strbuf_new();

 finish_init:
   return ++_ertf_initcount;
}

int
ertf_shutdown(void)
{
  if (_ertf_initcount != 1) goto finish_shutdown;

  _ertf_cleanup();
  //eina_strbuf_free(markup_buf);
  //eina_strbuf_free(style_buf);
  eina_log_domain_unregister(_ertf_log_dom);
  _ertf_log_dom = -1;
  eina_shutdown();

 finish_shutdown:
  return --_ertf_initcount;
}

void
ertf_default_color_set(unsigned char r, unsigned char g, unsigned char b)
{
  _ertf_default_color_r = r;
  _ertf_default_color_g = g;
  _ertf_default_color_b = b;
}

void
ertf_twip_scale_factor_set(int a)
{
  _twip_scale_factor = a;
}

static void
_ertf_cleanup()
{
  if (color_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Color         *color;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(color_table, i, color, iterator)
      free(color);
    eina_array_free(color_table);
  }
  if (font_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Font_Node     *font;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(font_table, i, font, iterator)
      free(font);
    eina_array_free(font_table);
  }
  if (stylesheet_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Stylesheet    *stylesheet;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(stylesheet_table, i, stylesheet, iterator)
      free(stylesheet);
    eina_array_free(stylesheet_table);
  }
  // todo: add summary info cleanup
}
