#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <Ecore_Evas.h>

#include "Ertf.h"


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
  int                   w, h, dpi;
  char                 *s;
  char                 *markup_text;
  Evas_Coord            x,y,h1,w1;

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

/*   ecore_evas_title_set(ee, "Ertf Evas test"); */
  ecore_evas_callback_delete_request_set(ee, _ertf_cb_delete);

  evas = ecore_evas_get(ee);

  doc = ertf_document_new();
  if (!doc)
    goto shutdown_ertf;

  if (!ertf_document_filename_set(doc, argv[1]))
    goto free_doc;

  if (!ertf_document_header_get(doc))
    goto free_doc;

#ifdef USE_DPI
  dpi = ecore_x_dpi_get();
  ertf_twip_scale_factor_set(dpi);
#endif
  
  ertf_document_size_get(doc, &w, &h);
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  if (!ertf_document_parse(doc))
    goto free_doc;

  printf ("Filename : %s\n", ertf_document_filename_get(doc));
  printf ("Version  : %d\n", ertf_document_version_get(doc));
  // printf ("markup   : %s\n", doc->markup);

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

  // todo create a member 'style' in the document structure and use that
  s = ertf_textblock_style_get();
  printf("%s\n", s);
  evas_textblock_style_set(st, s);
  evas_object_name_set(textblock, "textblock");
  evas_object_textblock_style_set(textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(textblock);
  markup_text = ertf_document_markup_get(doc);
  evas_object_textblock_text_markup_set(textblock, markup_text);
  evas_object_move(textblock, 0, 0);
  evas_object_resize(textblock, w, h);
  evas_object_show(textblock);

  // trial code begin
  printf("%d %d\n", w, h);
  evas_object_textblock_size_formatted_get(textblock, &w, &h);
  printf("%d %d\n", w, h);
  if(evas_object_textblock_line_number_geometry_get(textblock, 5, &x, &y, &w1, &h1))
    printf("%d %d %d %d\n", x, y, w1, h1);
  // trial code end

  ecore_main_loop_begin ();

  ertf_document_free(doc);
  ertf_shutdown();
  ecore_evas_shutdown();
  ecore_x_shutdown();

  return EXIT_SUCCESS;

 free_doc:
  ertf_document_free(doc);
 shutdown_ertf:
  ertf_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();
  ecore_x_shutdown();

  return EXIT_FAILURE;
}
