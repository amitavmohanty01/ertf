#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include <Eina.h>

#include "ertf_main.h"

unsigned char _ertf_default_color_r = 0;
unsigned char _ertf_default_color_g = 0;
unsigned char _ertf_default_color_b = 0;

/* The following values are in twips. */
extern int _ertf_default_paper_width   = 12240;
extern int _ertf_default_paper_height  = 15840;
extern int _ertf_default_left_margin   = 1800;
extern int _ertf_default_right_margin  = 1800;
extern int _ertf_default_top_margin    = 1440;
extern int _ertf_default_bottom_margin = 1440;

static int _ertf_initcount = 0;

int
ertf_init(void)
{
  if (_ertf_initcount)
    goto finish_init;

  if (!eina_init())
  {
    fprintf(stderr, "Could not initialize eina array module.\n");
    return 0;
  }

 finish_init:
   return ++_ertf_initcount;
}

int
ertf_shutdown(void)
{
  if (_ertf_initcount != 1) goto finish_shutdown;

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