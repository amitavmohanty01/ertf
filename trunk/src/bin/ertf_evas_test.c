#include <stdlib.h>
#include <stdio.h>

#include <Ecore_Evas.h>

#include "Ertf.h"
#include "ertf_private.h"


static void
_ertf_cb_delete (Ecore_Evas *ee)
{
  ecore_main_loop_quit();
}

int
main(int argc, char *argv[])
{
  Ecore_Evas           *ee;
  Evas                 *evas;
  Evas_Object          *textblock;
  Evas_Object          *background;
  Evas_Textblock_Style *st;
  Ertf_Document        *doc;

  if (argc < 2)
    {
      printf ("Usage: %s file.rtf\n", argv[0]);
      return EXIT_FAILURE;
    }

  if (!ecore_evas_init())
    return EXIT_FAILURE;

  if (!ertf_init())
    goto shutdown_ecore_evas;

  ee = ecore_evas_new("software_x11", 0, 0, 1024, 768, NULL);
  if (!ee)
    goto shutdown_ertf;

  ecore_evas_title_set(ee, "Ertf Smart test");
  ecore_evas_callback_delete_request_set(ee, _ertf_cb_delete);
  ecore_evas_show(ee);

  evas = ecore_evas_get(ee);

  doc = ertf_document_new();
  if (!doc)
    goto shutdown_ertf;

  if (!ertf_document_filename_set(doc, argv[1]))
    goto free_doc;

  if (!ertf_document_header_get(doc))
    goto free_doc;

  if (!ertf_document_parse(doc))
    goto free_doc;

  printf ("Filename : %s\n", ertf_document_filename_get(doc));
  printf ("Version  : %d\n", ertf_document_version_get(doc));
  printf ("Charset  : %s\n", ertf_document_charset_get(doc));

  /* background */
  background = evas_object_rectangle_add(evas);
  evas_object_color_set(background, 255, 255, 255, 255);
  evas_object_move(background, 0, 0);
  evas_object_resize(background, 1024, 768);
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
  evas_object_resize(textblock, 1024, 768);
  evas_object_show(textblock);

  ecore_main_loop_begin ();

  ertf_document_free(doc);
  ertf_shutdown();
  ecore_evas_shutdown();

  return EXIT_SUCCESS;

 free_doc:
  ertf_document_free(doc);
 shutdown_ertf:
  ertf_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();

  return EXIT_FAILURE;
}
