#include "GUI.h"
#include "input.h"
#include <stdlib.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

int init_gui(){
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *textblock;
  Evas_Object *background;
  Evas_Textblock_Style *st;

  /* initialize libraries */
  ecore_evas_init();

  /* create our Ecore_Evas and show it */
  ee = ecore_evas_new("software_x11", 0, 0, 800, 600, NULL);
  ecore_evas_title_set(ee, "Ertf");
  ecore_evas_show(ee);

  /* get a pointer our new Evas canvas */
  evas = ecore_evas_get(ee);

  /* background */
  background = evas_object_rectangle_add(evas);
  evas_object_color_set(background, 255, 255, 255, 255);
  evas_object_move(background, 0, 0);
  evas_object_resize(background, 800, 600);
  evas_object_name_set(background, "background");
  evas_object_show(background);

  /* create an textblock object */
  textblock = evas_object_textblock_add(evas);
  st = evas_textblock_style_new();

  // todo: remove the name if not required
  evas_object_name_set(textblock, "textblock");

  // todo: replace the string by style_string defined in GUI.h
  evas_textblock_style_set(st,
			   "DEFAULT='font=Vera,Kochi font_size=8 align=left color=#000000 wrap=word'"
			   "center='+ font=Vera,Kochi font_size=10 align=center'"
			   "/center='- \n'"
			   "right='+ font=Vera,Kochi font_size=10 align=right'"
			   "/right='- \n'"
			   "blockquote='+ left_margin=+24 right_margin=+24 font=Vera,Kochi font_size=10 align=left'"
			   "h1='+ font_size=20'"
			   "red='+ color=#ff0000'"
			   "p='+ font=Vera,Kochi font_size=10 align=left'"
			   "/p='- \n'"
			   "br='\n'"
			   "tab='\t'"
			   );
  evas_object_name_set(textblock, "textblock");
  evas_object_textblock_style_set(textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(textblock);
  evas_object_textblock_text_markup_set(textblock, markup);
  evas_object_move(textblock, 0, 0);
  evas_object_resize(textblock, 800, 600);
  evas_object_show(textblock);

  /* todo: add callbacks as required */
  
  /* start the main event loop */
  ecore_main_loop_begin();

  return 0;
}

int shutdown_gui(){
  /* when the main event loop exits, shutdown our libraries */
  ecore_evas_shutdown();
  return 1;// indicates success
}
