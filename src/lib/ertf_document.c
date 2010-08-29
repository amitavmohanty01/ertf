#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Eina.h>
#include <Evas.h>

#include "ertf_document.h"
#include "ertf_summary.h"
#include "ertf_input.h"
#include "ertf_color.h"
#include "ertf_font.h"
#include "ertf_stylesheet.h"
#include "ertf_private.h"

Ertf_Document *
ertf_document_new(const char *filename)
{
  Ertf_Document *doc;
  char str[5];
  int  c;

  if (!filename || (*filename == '\0'))
    return NULL;

  doc = (Ertf_Document *)calloc(1, sizeof(Ertf_Document));
  if (!doc)
    return NULL;

  doc->filename = strdup(filename);
  if (!doc->filename)
    goto free_doc;

  doc->stream = fopen(doc->filename, "rb");
  if (!doc->stream)
  {
    ERR("error opening file");
    goto free_filename;
  }

  doc->bracecount = 0;

  while ((c = getc(doc->stream)) != EOF)
  {
    switch (c)
    {
      char tag [30];
    case '{':
      if (doc->bracecount)
      {
	ungetc(c, doc->stream);
	goto create_array;
      }
      else
	doc->bracecount++;
      break;

    case '\\':
      if(ertf_tag_get(doc->stream, tag))
      {
	ERR("encountered EOF while reading control tag");
	goto close_stream;
      }
      if (strcmp(tag, "rtf") == 0)
	fscanf(doc->stream, "%d", &doc->version);
      /* charset tags */
      else if (strcmp(tag, "ansi") == 0)
      {
	doc->charset = ERTF_DOCUMENT_CHARSET_ANSI;
      }
      else if (strcmp(tag, "mac") == 0)
      {
	doc->charset = ERTF_DOCUMENT_CHARSET_MAC;
      }
      else if (strcmp(tag, "pc") == 0)
      {
	doc->charset = ERTF_DOCUMENT_CHARSET_PC;
      }
      else if (strcmp(tag, "pca") == 0)
      {
	doc->charset = ERTF_DOCUMENT_CHARSET_PCA;
      }
      /* default font */
      else if (strcmp(tag, "deff") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_default_font);      
      }
      else
      {
	DBG("unrecognised control tag %s", tag);
      }
      break;

    default:
      if (c == '\n' || c == '\r')
	_line++;
      DBG("unrecognised control character '%c'", c);
    }
  }
  ERR("encountered EOF while parsing header.\n");
  goto close_stream;

 create_array:
  doc->pages = eina_array_new(10);
  if (!doc->pages)
    goto close_stream;

  doc->version = -1;

  return doc;

 close_stream:
  fclose(doc->stream);
 free_filename:
  free(doc->filename);
 free_doc:
  free(doc);

  return NULL;
}

void
ertf_document_free(Ertf_Document *doc)
{
  unsigned int        i;
  Eina_Array_Iterator iterator;
  char               *page;

  if (!doc)
    return;

  if (doc->stream)
    fclose(doc->stream);
  if (doc->filename)
    free(doc->filename);
  if (doc->markup)
    free(doc->markup);
  if (doc->style)
    free(doc->style);
  if (doc->summary)
  {
    free(doc->summary->author);
    free(doc->summary);
  }
  EINA_ARRAY_ITER_NEXT(doc->pages, i, page, iterator)
  {
    free(page);
  }
  eina_array_free(doc->pages);
  free(doc);
}

const char *
ertf_document_filename_get(Ertf_Document *doc)
{
  if (!doc)
    return NULL;

  return doc->filename;
}

int
ertf_document_parse(Ertf_Document *doc)
{
  char control_word[30];
  int  c;

  if (!doc || !doc->stream)
    return 0;

  while ((c = getc(doc->stream)) != EOF)
  {    
    switch (c)
    {
    case '{':
      doc->bracecount++;
      break;
    case '}':
      doc->bracecount--;
      break;
    case '\\'://todo:perform the control operation
      if (ertf_tag_get(doc->stream, control_word))
      {
        ERR("EOF encountered");
        return 0;
      }

      /* font table */
      if (strcmp(control_word, "fonttbl") == 0)
      {
	if (ertf_font_table(doc->stream))
        {	
	  DBG("Successfully created font table");
	  doc->bracecount--;
	}
        else
	  DBG("failure in creating font table");

	/* color table */
      }
      else if (strcmp(control_word, "colortbl") == 0)
      {
	if (ertf_color_table(doc->stream))
        {
	  DBG("Successfully created color table");
	  doc->bracecount--;
	}
        else
	  DBG("failure in creating color table");

	/* stylesheet */
      }
      else if (strcmp(control_word, "stylesheet") == 0)
      {
	if (ertf_stylesheet_parse(doc->stream))
        {
	  DBG("Successfully created stylesheet table");
	  doc->style = eina_strbuf_string_get(style_buf);
	  doc->bracecount--;
	}
        else
	  DBG("failure in creating stylesheet table");
      }

      /* paragraph */
      else if (strcmp(control_word, "pard") == 0)
      {
	int cc;
	while ((cc = fgetc(doc->stream)) != EOF || cc != '}')
	{
	  eina_strbuf_append(markup_buf, "<p>");
	  ungetc(cc, doc->stream);
	  if (ertf_paragraph_translate(doc->stream, 0))
	  {
	    DBG("Successfully parsed a paragraph");
	  }
	  else
	  {
	    DBG("failure parsing parapgraph");
	    break;
	  }
	}
	doc->bracecount--;
      }

      /* unsupported group */
      else if (strcmp(control_word, "*") == 0)
      {
	if (ertf_group_skip(doc->stream))
	  ERR("EOF encountered while skipping group");
      }

      /* handle paper height */
      else if (strcmp(control_word, "paperh") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_paper_height);
      }

      /* handle paper width */
      else if (strcmp(control_word, "paperw") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_paper_width);
      }

      /* handle left margin */
      else if (strcmp(control_word, "margl") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_margin_left);
      }

      /* handle right margin */
      else if (strcmp(control_word, "margr") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_margin_right);
      }

      /* handle top margin */
      else if (strcmp(control_word, "margt") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_margin_top);
      }

      /* handle bottom margin */
      else if (strcmp(control_word, "margb") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_margin_bottom);
      }

      /* default tab size in twips */
      else if (strcmp(control_word, "deftab") == 0)
      {
	fscanf(doc->stream, "%d", &c);
	// todo: look for possible implementation in textblock, otherwise the tag is useless.
      }

      else if (strcmp(control_word, "info") == 0)
      {
	if (ertf_summary(doc))
	  DBG("Successfully parsed information section");
	else
	  DBG("failure parsing information about the file");
      }

      /* unrecognised control word */
      else
      {
	DBG("skipping unrecognised control word %s", control_word);
      }

      break;

    default:
      if (c == '\n' || c == '\r')
	_line++;
      DBG("skipped control char `%c'", c);
    }
  }

  doc->markup = eina_strbuf_string_get(markup_buf);

  if (doc->bracecount)
    WARN("Ill-formed rtf - inconsistent use of braces");

  return 1;
}

int
ertf_document_page_count_get (const Ertf_Document *document)
{
  if (!document)
    return 0;

  return eina_array_count_get(document->pages);
}

int
ertf_document_version_get(Ertf_Document *doc)
{
  if (!doc)
    return -1;

  return doc->version;
}

enum Ertf_Document_Charset
ertf_document_charset_get(Ertf_Document *doc)
{
  if (!doc)
    return ERTF_DOCUMENT_CHARSET_UNKNOWN;

  return doc->charset;
}

void
ertf_document_size_get(Ertf_Document *doc, int *width, int *height)
{
  if (!doc)
    return;

  if (width)
    *width = (int) ceilf(_ertf_paper_width / 1440.0f * _twip_scale_factor);
  if (height)
    *height = (int) ceilf(_ertf_paper_height / 1440.0f * _twip_scale_factor);
}

void
ertf_document_margin_get(Ertf_Document *doc, int *left, int *right, int *top, int *bottom)
{
  if (!doc)
    return;

  if (left)
    *left = (int) ceilf(_ertf_margin_left / 1440.0f * _twip_scale_factor);
  if (right)
    *right = (int) ceilf(_ertf_margin_right / 1440.0f * _twip_scale_factor);
  if (top)
    *top = (int) ceilf(_ertf_margin_top / 1440.0f * _twip_scale_factor);
  if (bottom)
    *bottom = (int) ceilf(_ertf_margin_bottom / 1440.0f * _twip_scale_factor);
}
