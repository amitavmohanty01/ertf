#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include <eina_array.h>

#include "ertf_main.h"

unsigned char _ertf_default_color_r = 0;
unsigned char _ertf_default_color_g = 0;
unsigned char _ertf_default_color_b = 0;

static int _ertf_initcount = 0;

int
ertf_init(void)
{
  if (_ertf_initcount)
    goto finish_init;

  if (!eina_array_init())
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

  eina_array_shutdown();

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
