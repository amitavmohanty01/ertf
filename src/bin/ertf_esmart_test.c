#include <stdlib.h>
#include <stdio.h>

#include <Ecore_Evas.h>

#include "Ertf.h"
#include "esmart_rtf.h"


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
  Evas_Textblock_Style *st;

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

  o = esmart_rtf_add(evas);
  esmart_rtf_file_set (o, argv[1]);
  evas_object_move (o, 0, 0);
  evas_object_resize (o, 1024, 768);
  evas_object_show (o);

  ecore_main_loop_begin ();

  ertf_shutdown();
  ecore_evas_shutdown();

  return EXIT_SUCCESS;

 shutdown_ertf:
  ertf_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();

  return EXIT_FAILURE;
}
