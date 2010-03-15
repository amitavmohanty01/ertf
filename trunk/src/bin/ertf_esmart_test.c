#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <Ecore_Evas.h>

#include "Ertf.h"
#include "esmart_rtf.h"
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
  Evas_Object          *o;
  Evas_Object *bg;
  char        *filename;
  int          page_number;
  int                   w, h, dpi;

  if (argc < 3)
    {
      printf ("Usage: %s file.rtf page_number\n\n", argv[0]);
      return EXIT_FAILURE;
    }

  filename = argv[1];
  sscanf (argv[2], "%d", &page_number);

  if (!ecore_evas_init())
    return EXIT_FAILURE;

  if (!ertf_init())
    goto shutdown_ecore_evas;

  ee = ecore_evas_new("software_x11", 0, 0, 1024, 768, NULL);
  if (!ee)
    goto shutdown_ertf;

  ecore_evas_title_set(ee, "Ertf Smart test");
  ecore_evas_callback_delete_request_set(ee, _ertf_cb_delete);

  evas = ecore_evas_get(ee);

  bg = evas_object_rectangle_add(evas);
  evas_object_color_set(bg, 255, 255, 255, 255);
  evas_object_move(bg, 0, 0);
  evas_object_show(bg);

  o = esmart_rtf_add(evas);

  if (!esmart_rtf_file_set (o, filename)) {
    printf ("1\n");
    evas_object_del (o);
    ecore_evas_shutdown ();
    return EXIT_FAILURE;
  }

  printf ("2\n");
  esmart_rtf_page_set(o, page_number);
  esmart_rtf_render (o);
  evas_object_move (o, 0, 0);
  evas_object_show (o);

  esmart_rtf_size_get(o, &w, &h);
  evas_object_resize(bg, w, h);
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  ecore_main_loop_begin ();

  evas_object_del (o);
  ecore_evas_shutdown();

  return EXIT_SUCCESS;

 shutdown_ertf:
  ertf_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();

  return EXIT_FAILURE;
}
