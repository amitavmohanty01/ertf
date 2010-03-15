#ifndef __ESMART_RTF_H__
#define __ESMART_RTF_H__


Evas_Object *esmart_rtf_add (Evas *evas);

Eina_Bool esmart_rtf_file_set(Evas_Object *obj, const char *filename);

const char *esmart_rtf_file_get(Evas_Object *obj);

void esmart_rtf_page_set (Evas_Object *obj, int page);

int esmart_rtf_page_get(Evas_Object *obj);

void esmart_rtf_size_get(Evas_Object *obj, int *width, int *height);

void esmart_rtf_render (Evas_Object *obj);


#endif /* __ESMART_RTF_H__ */
