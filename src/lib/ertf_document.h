#ifndef __ERTF_DOCUMENT_H__
#define __ERTF_DOCUMENT_H__


typedef struct Ertf_Document Ertf_Document;

typedef enum Ertf_Document_Charset
{
  ERTF_DOCUMENT_CHARSET_ANSI,
  ERTF_DOCUMENT_CHARSET_MAC,
  ERTF_DOCUMENT_CHARSET_PC,
  ERTF_DOCUMENT_CHARSET_PCA,
  ERTF_DOCUMENT_CHARSET_UNKNOWN
} Ertf_Document_Charset;

Ertf_Document *ertf_document_new(const char *filename);

void ertf_document_free(Ertf_Document *doc);

const char *ertf_document_filename_get(Ertf_Document *doc);

int ertf_document_version_get(Ertf_Document *doc);

enum Ertf_Document_Charset ertf_document_charset_get(Ertf_Document *doc);

int ertf_document_parse(Ertf_Document *doc);

int ertf_document_page_count_get (const Ertf_Document *document);

void ertf_document_size_get(Ertf_Document *doc, int *width, int *height);

void ertf_document_margin_get(Ertf_Document *doc, int *left, int *right, int *top, int *bottom);


#endif /* __ERTF_DOCUMENT_H__ */
