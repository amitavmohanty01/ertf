#ifndef __ERTF_PAGE_H__
#define __ERTF_PAGE_H__

#include <Evas.h>

typedef struct Ertf_Page Ertf_Page;

Ertf_Page *ertf_page_new (const Ertf_Document *doc);

void ertf_page_free (Ertf_Page *page);

void ertf_page_render (Ertf_Page *page, Evas_Object *textblock);

void ertf_page_page_set (Ertf_Page *page, int p);

int ertf_page_page_get (Ertf_Page *page);


#endif /* __ERTF_PAGE_H__ */
