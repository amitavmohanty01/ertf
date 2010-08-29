#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <Ecore.h>
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
  Ertf_Page             *epage;
  int                    w, h, dpi;
  int                    pageno;

  if (argc < 3)
  {
    printf ("Usage: %s file.rtf <page_number>\n", argv[0]);
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

  doc = ertf_document_new(argv[1]);
  if (!doc)
    goto shutdown_ertf;

  epage = ertf_page_new(doc);
  if (!epage)
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
    goto free_page;

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
  sscanf(argv[2], "%d", &pageno);
  ertf_page_page_set(epage, pageno);
  ertf_page_render(epage, textblock);
  evas_object_move(textblock, 0, 0);
  evas_object_resize(textblock, w, h);
  evas_object_show(textblock);

  ecore_main_loop_begin ();

  ertf_page_free(epage);
  ertf_document_free(doc);
  ertf_shutdown();
  ecore_evas_shutdown();

  return EXIT_SUCCESS;

 free_page:
  ertf_page_free(epage);
 free_doc:
  ertf_document_free(doc);
 shutdown_ertf:
  ertf_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();
 
  return EXIT_FAILURE;
}
