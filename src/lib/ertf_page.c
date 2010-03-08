#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "Ertf.h"
#include "ertf_page.h"
#include "ertf_private.h"


Ertf_Page *
ertf_page_new(const Ertf_Document *doc)
{
  Ertf_Page *page;

  page = (Ertf_Page *)malloc(sizeof(Ertf_Page));
  if (!page)
    return NULL;

  page->doc = doc;
  page->page = 1;

  return page;
}

void
ertf_page_free(Ertf_Page *page)
{
  if (!page)
    return;

  free(page->doc);
  free(page);
}

void
ertf_page_page_set(Ertf_Page *page, int p)
{
  page->page = p;
}

int
ertf_page_page_get(Ertf_Page *page)
{
  return page->page;
}

void 
ertf_page_render (Ertf_Page *page, Evas *evas)
{
  Evas_Object          *textblock;
  Evas_Textblock_Style *st;
  char                 *s;
  char                 *markup_text;

  textblock = evas_object_textblock_add(evas);
  st = evas_textblock_style_new();
  s = ertf_document_style_get(page->doc);
  evas_textblock_style_set(st, s);
  evas_object_textblock_style_set(textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(textblock);
  markup_text = eina_array_data_get(page->doc->pages, page->page);
  evas_object_textblock_text_markup_set(textblock, markup_text);
}
