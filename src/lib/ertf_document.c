#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ertf_document.h"
#include "ertf_summary.h"
#include "ertf_input.h"
#include "ertf_private.h"


Ertf_Document *
ertf_document_new(void)
{
  Ertf_Document *doc;

  doc = (Ertf_Document *)calloc(1, sizeof(Ertf_Document));
  if (!doc)
    return NULL;

  doc->version = -1;

  return doc;
}

void
ertf_document_free(Ertf_Document *doc)
{
  if (!doc)
    return;

  if (doc->stream)
    fclose(doc->stream);
  if (doc->filename)
    free(doc->filename);
  if (doc->markup)
    free(doc->markup);
  if (doc->summary)
  {
    free(doc->summary->author);
    free(doc->summary);
  }
  free(doc);
}

int
ertf_document_filename_set(Ertf_Document *doc, const char *filename)
{
  if (!doc || !filename || (*filename == '\0'))
    return 0;

  if ((doc->filename) &&
      (!strcmp (filename, doc->filename)))
    return 1;

  if (doc->filename)
  {
    free(doc->filename);
    doc->filename = NULL;
  }
  if (doc->stream)
  {
    fclose(doc->stream);
    doc->stream = NULL;
  }

  doc->filename = strdup(filename);
  if (!doc->filename)
    return 0;

  doc->stream = fopen(doc->filename, "rb");
  if (!doc->stream)
  {
    free(doc->filename);
    doc->filename = NULL;
    return 0;
  }

  return 1;
}

const char *
ertf_document_filename_get(Ertf_Document *doc)
{
  if (!doc)
    return NULL;

  return doc->filename;
}

int
ertf_document_header_get(Ertf_Document *doc)
{
  char str[5];
  int  c;

  if (!doc || !doc->stream)
    return 0;

  while ((c = getc(doc->stream)) != EOF)
  {
    switch (c)
    {
      char tag [30];
    case '{':
      if (doc->bracecount)
      {
	ungetc(c, doc->stream);
	return 1;
      }
      else
	doc->bracecount++;
      break;

    case '\\':
      if(ertf_tag_get(doc->stream, tag))
      {
	ERR("ertf_document_header_get: encountered EOF while reading control tag");
	return 0;
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
	DBG("ertf_document_header_get: unrecognised control tag %s", tag);
      }
      break;
    default:
      DBG("ertf_document_header_get: unrecognised control character %c", c);
    }
  }
  ERR("ertf_document_header_get: encountered EOF while parsing header.\n");
  return 0;
}

int
ertf_document_parse(Ertf_Document *doc)
{
  char control_word[30];
  int  c;

  if (!doc || !doc->stream)
    return 0;

  ertf_markup_position = 0;
  markup = NULL;

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
        ERR("ertf_document_parse: EOF encountered");
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
	  doc->bracecount--;
	}
        else
	  DBG("failure in creating stylesheet table");
      }

      /* paragraph */
      else if (strcmp(control_word, "pard") == 0)
      {
	int c;
	while ((c = fgetc(doc->stream)) != EOF || c != '}')
	{
	  ertf_markup_add("<p>", 3);
	  ungetc(c, doc->stream);
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
	  ERR("ertf_document_parse: EOF encountered while skipping group");
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
	DBG("ertf_document_parse: skipping unrecognised control word %s", control_word);
      }

      break;

    default:
      DBG("ertf_document_parse: skipped control char `%c'", c);
    }
  }

  markup[ertf_markup_position] = '\0';
  doc->markup = markup;
  markup = NULL;
  printf("%d\nmarkup:\n%s\n", ertf_markup_position, doc->markup);
  // When end-of-file is reached, check if  parsing is complete. In case,
  // it is not, print an error message stating "incomplete rtf file".
  if (doc->bracecount)
    WARN("ertf_document_parse: Ill-formed rtf - inconsistent use of braces");

  return 1;
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

char *
ertf_document_markup_get(Ertf_Document *doc)
{
  if (!doc)
    return NULL;
  return doc->markup;
}
