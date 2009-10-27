#include <stdlib.h>
#include <math.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Ertf.h"
#include "GUI.h"
#include "ertf_private.h"


static void
_cb_delete (Ecore_Evas *ee)
{
  ecore_main_loop_quit();
}

int
init_gui()
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *textblock;
  Evas_Object *background;
  Evas_Textblock_Style *st;
  char *s;
  int w, h, dpi;

  /* initialize libraries */
  ecore_evas_init();

  /* create our Ecore_Evas and show it */
  ee = ecore_evas_new("software_x11", 0, 0, 1024, 768, NULL);
  ecore_evas_title_set(ee, "Ertf");
  ecore_evas_callback_delete_request_set(ee, _cb_delete);

  /* calculate textblock size */
  // todo: actually this should be screen size. if the textblock size is more, scroll bars should be supplied.
  dpi = ecore_x_dpi_get();
  w = (int) ceilf(_ertf_paper_width / 1440.0f * dpi);
  h = (int) ceilf(_ertf_paper_height / 1440.0f * dpi);  
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  /* get a pointer our new Evas canvas */
  evas = ecore_evas_get(ee);

  /* background */
  background = evas_object_rectangle_add(evas);
  evas_object_color_set(background, 255, 255, 255, 255);
  evas_object_move(background, 0, 0);
  evas_object_resize(background, w, h);
  evas_object_name_set(background, "background");
  evas_object_show(background);

  /* create an textblock object */
  textblock = evas_object_textblock_add(evas);
  st = evas_textblock_style_new();

  // todo: remove the name if not required
  evas_object_name_set(textblock, "textblock");

  s = ertf_textblock_style_get();
  printf("%s\n", s);  
  evas_textblock_style_set(st, s);
  evas_object_textblock_style_set(textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(textblock);
  evas_object_textblock_text_markup_set(textblock, markup);
  evas_object_move(textblock, 0, 0);
  evas_object_resize(textblock, w, h);
  evas_object_show(textblock);
  
  /* start the main event loop */
  ecore_main_loop_begin();

  return 0;
}

int
shutdown_gui(){
  /* when the main event loop exits, shutdown our libraries */
  ecore_evas_shutdown();
  return 1;// indicates success
}