#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include <Evas.h>

#include "ertf_document.h"
#include "ertf_page.h"


#define E_SMART_OBJ_GET(smart, o, type) \
  { \
    char *_e_smart_str; \
    \
    if (!o) return; \
    \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return; \
    \
    _e_smart_str = (char *)evas_object_type_get(o); \
    if (!_e_smart_str) return; \
    \
    if (strcmp(_e_smart_str, type)) return; \
  }

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret) \
  { \
    char *_e_smart_str; \
    \
    if (!o) return ret; \
    \
    smart = evas_object_smart_data_get(o); \
    if (!smart) return ret; \
    \
    _e_smart_str = (char *)evas_object_type_get(o); \
    if (!_e_smart_str) return ret; \
    \
    if (strcmp(_e_smart_str, type)) return ret; \
  }

#define E_OBJ_NAME "rtf_object"


typedef struct _Smart_Rtf Smart_Rtf;

struct _Smart_Rtf
{
  Evas_Object *obj;

  Ertf_Document *rtf_document;
  Ertf_Page *rtf_page;
};

/* local subsystem functions */
static void _smart_reconfigure(Smart_Rtf *sd);
static void _smart_init(void);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object *obj);

/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

Evas_Object *
esmart_rtf_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, _e_smart);
}

Eina_Bool
esmart_rtf_file_set(Evas_Object *obj, const char *filename)
{
  Smart_Rtf *sr;

  if (!filename || (*filename == '\0'))
    return EINA_FALSE;

  E_SMART_OBJ_GET_RETURN (sr, obj, E_OBJ_NAME, 0);

  if (sr->rtf_page)
    ertf_page_free (sr->rtf_page);
  sr->rtf_page = NULL;

  if (sr->rtf_document)
    ertf_document_free (sr->rtf_document);

  sr->rtf_document = ertf_document_new (filename);
  if (!sr->rtf_document)
    return EINA_FALSE;

  ertf_document_parse(sr->rtf_document);

  sr->rtf_page = ertf_page_new (sr->rtf_document);

  return EINA_TRUE;
}

const char *
esmart_rtf_file_get(Evas_Object *obj)
{
  Smart_Rtf *sr;

  E_SMART_OBJ_GET_RETURN (sr, obj, E_OBJ_NAME, NULL);

  return ertf_document_filename_get(sr->rtf_document);
}

void
esmart_rtf_page_set (Evas_Object *obj, int page)
{
  Smart_Rtf *sr;

  E_SMART_OBJ_GET(sr, obj, E_OBJ_NAME);

  ertf_page_page_set (sr->rtf_page, page);
}

int
esmart_rtf_page_get(Evas_Object *obj)
{
  Smart_Rtf *sr;

  E_SMART_OBJ_GET_RETURN(sr, obj, E_OBJ_NAME, 0);

  return ertf_page_page_get (sr->rtf_page);
}

void
esmart_rtf_size_get(Evas_Object *obj, int *width, int *height)
{
  Smart_Rtf *sr;

  E_SMART_OBJ_GET(sr, obj, E_OBJ_NAME);

   if (!sr)
   {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
   }

   ertf_document_size_get (sr->rtf_document, width, height);
}

void
esmart_rtf_render (Evas_Object *obj)
{
  Smart_Rtf *sr;
  int width;
  int height;

  E_SMART_OBJ_GET (sr, obj, E_OBJ_NAME);

  if (sr->rtf_document && sr->rtf_page && sr->obj)
    {
      ertf_document_size_get(sr->rtf_document, &width, &height);
      ertf_page_render (sr->rtf_page, sr->obj);
      evas_object_resize (sr->obj, width, height);
    }
}


/* local subsystem globals */

static void
_smart_init(void)
{
  if (_e_smart) return;

  printf ("%s\n", __FUNCTION__);
  static const Evas_Smart_Class sc = {
    E_OBJ_NAME,
    EVAS_SMART_CLASS_VERSION,
    _smart_add,
    _smart_del,
    _smart_move,
    _smart_resize,
    _smart_show,
    _smart_hide,
    _smart_color_set,
    _smart_clip_set,
    _smart_clip_unset,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  };
  _e_smart = evas_smart_class_new(&sc);
}

static void
_smart_add(Evas_Object *obj)
{
  Smart_Rtf            *sr;
  Evas                 *evas;
  Evas_Textblock_Style *st;

  sr = calloc(1, sizeof(Smart_Rtf));
  if (!sr) return;

  evas = evas_object_evas_get(obj);
  sr->obj = evas_object_textblock_add(evas);
  evas_object_smart_member_add(sr->obj, obj);
  evas_object_smart_data_set(obj, sr);
}

static void
_smart_del(Evas_Object *obj)
{
  Smart_Rtf         *sr;
  Eina_Array_Iterator iterator;
  unsigned int        i;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;

  if (sr->rtf_document)
    ertf_document_free (sr->rtf_document);
  if (sr->rtf_page)
    ertf_page_free (sr->rtf_page);

  ertf_shutdown();

  free(sr);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;

  evas_object_move (sr->obj, x, y);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;

  //   if ((sr->w == w) && (sr->h == h)) return;
  evas_object_resize (sr->obj, w, h);
}

static void
_smart_show(Evas_Object *obj)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;
  evas_object_show(sr->obj);
}

static void
_smart_hide(Evas_Object *obj)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;
  evas_object_hide(sr->obj);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;
  evas_object_color_set(sr->obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object * clip)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;
  evas_object_clip_set(sr->obj, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
  Smart_Rtf *sr;

  sr = evas_object_smart_data_get(obj);
  if (!sr) return;
  evas_object_clip_unset(sr->obj);
}
