#ifndef __ERTF_DOCUMENT_H__
#define __ERTF_DOCUMENT_H__


typedef struct Ertf_Document Ertf_Document;


Ertf_Document *ertf_document_new(void);

void ertf_document_free(Ertf_Document *doc);

int ertf_document_filename_set(Ertf_Document *doc, const char *filename);

const char *ertf_document_filename_get(Ertf_Document *doc);

int ertf_document_header_get(Ertf_Document *doc);

int ertf_document_version_get(Ertf_Document *doc);

const char *ertf_document_charset_get(Ertf_Document *doc);

int ertf_document_parse(Ertf_Document *doc);


#endif /* __ERTF_DOCUMENT_H__ */
