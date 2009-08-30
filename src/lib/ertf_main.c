#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include <eina_array.h>

#include "ertf_main.h"

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
