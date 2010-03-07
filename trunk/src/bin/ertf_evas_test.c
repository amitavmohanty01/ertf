#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

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
  Ecore_Evas            *ee;
  Evas                  *evas;
  Evas_Object           *textblock;
  Evas_Object           *background;
  Evas_Textblock_Style  *st;
  Ertf_Document         *doc;
  int                    w, h, dpi, line_number;
  char                  *s;
  char                  *markup_text;
  Evas_Coord             x, y, h1, w1;
  Evas_Textblock_Cursor *c1, *c2;
  int                    page = 1, last_page = 0;
  int ln;

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

  ecore_evas_title_set(ee, "Ertf Evas test"); 
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
  printf("Using dpi.\n");
#endif
  
  ertf_document_size_get(doc, &w, &h);
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  if (!ertf_document_parse(doc))
    goto free_doc;

  printf ("Filename : %s\n", ertf_document_filename_get(doc));
  printf ("Version  : %d\n", ertf_document_version_get(doc));  

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
  printf("style: %s\n", s);
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

  // paging code begin
  c1 = evas_object_textblock_cursor_new(textblock);
  evas_textblock_cursor_line_first(c1);
  evas_textblock_cursor_char_first(c1);

  while (!evas_textblock_cursor_node_format_get(c1))
    evas_textblock_cursor_node_next(c1);

  while (!evas_textblock_cursor_node_format_is_visible_get(c1))
    evas_textblock_cursor_node_next(c1);

  line_number = evas_textblock_cursor_line_geometry_get(c1, NULL, NULL, NULL, NULL);
  c2 = evas_object_textblock_cursor_new(textblock);
  h--;
  line_number = evas_textblock_cursor_line_coord_set(c2, h);  

  do
  {
    printf("line number: %d last page:%d\n", line_number, last_page);
    evas_textblock_cursor_char_last(c2);

    if (!evas_textblock_cursor_node_format_get(c2))
      printf("no format\n");

    if (evas_textblock_cursor_node_prev(c2))
    {
	while (!evas_textblock_cursor_node_format_is_visible_get(c2))
	  evas_textblock_cursor_node_prev(c2);
    }

    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    printf("%s\n", s);
    if (evas_textblock_cursor_node_format_is_visible_get(c2))
      printf("visible\n");
    else
      printf("invisible\n");
    /* instead of a free statement add proper page deletion code on deletion of document object */
    free(s);
    printf("line #%d page #%d\n", line_number, page);

    /* update */
    page++;
    evas_textblock_cursor_copy(c2, c1);
    evas_textblock_cursor_char_next(c1); // todo: check the return value
    line_number = evas_textblock_cursor_line_coord_set(c2, page * h);    
  } while (evas_textblock_cursor_compare(c1, c2) < 0);

  evas_textblock_cursor_node_last(c2);
  ln=evas_textblock_cursor_line_geometry_get(c1, NULL,NULL,NULL,NULL);

  if (ln > 0)
  {
    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    printf("%s\npage#%d\n", s, page);
    free(s);
  }

  evas_textblock_cursor_free(c2);
  evas_textblock_cursor_free(c1);
  // paging code end

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
