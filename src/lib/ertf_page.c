#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <Eina.h>

#include "Ertf.h"
#include "ertf_page.h"
#include "ertf_private.h"


static void
_ertf_document_generate_pages(Evas_Object *textblock, Ertf_Document *doc);

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

  free(page);
}

void
ertf_page_page_set(Ertf_Page *page, int p)
{
  /*  if (p < 0)
    p = 0;
  if (p >= eina_array_count_get(page->doc->pages))
  p = eina_array_count_get(page->doc->pages) - 1;*/
  page->page = p;
}

int
ertf_page_page_get(Ertf_Page *page)
{
  return page->page;
}

void 
ertf_page_render (Ertf_Page *page, Evas_Object *textblock)
{
  char *markup_text;

  if (eina_array_count_get(page->doc->pages) == 0)
  {
    _ertf_document_generate_pages(textblock, page->doc);
  }
  if (page->page < 0)		
    page->page = 0;
  if (page->page >= eina_array_count_get(page->doc->pages))		
    page->page = eina_array_count_get(page->doc->pages) - 1;
  evas_object_textblock_clear(textblock);
  markup_text = eina_array_data_get(page->doc->pages, page->page);
  evas_object_textblock_text_markup_set(textblock, markup_text);
}

static void
_ertf_document_generate_pages(Evas_Object *textblock, Ertf_Document *doc)
{
  Evas_Textblock_Style  *st;
  Evas_Textblock_Cursor *c1;
  Evas_Textblock_Cursor *c2;
  char                  *s;
  int                    ln;
  int                    w;
  int                    h;
  int                    page = 1;
  
  ertf_document_size_get(doc, &w, &h);
  st = evas_textblock_style_new();
  evas_textblock_style_set(st, doc->style);
  evas_object_textblock_style_set(textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(textblock);
  evas_object_textblock_text_markup_set(textblock, doc->markup);
  evas_object_resize(textblock, w, h);
  evas_object_show(textblock);

  c1 = evas_object_textblock_cursor_new(textblock);
  evas_textblock_cursor_paragraph_first(c1);

  c2 = evas_object_textblock_cursor_new(textblock);
    evas_textblock_cursor_line_coord_set(c2, h - 1);
  
  evas_textblock_cursor_paragraph_first(c1);
  evas_textblock_cursor_paragraph_char_first(c1);

  do
  {
    evas_textblock_cursor_line_char_last(c2);
    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    eina_array_push(doc->pages, s);

    page++;
    evas_textblock_cursor_copy(c2, c1);
    evas_textblock_cursor_char_next(c1);
    evas_textblock_cursor_line_coord_set(c2, page * h - 1);
  } while (evas_textblock_cursor_compare(c1, c2) < 0);

  evas_textblock_cursor_paragraph_last(c2);
  if (evas_textblock_cursor_compare(c1, c2) < 0)
  {
    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    eina_array_push(doc->pages, s);
  }

  evas_textblock_cursor_free(c2);
  evas_textblock_cursor_free(c1);
  //free(doc->markup);
  // causes double free error and core dumps
}
