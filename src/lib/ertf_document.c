#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ertf_document.h"
#include "ertf_input.h"
#include "ertf_private.h"


enum Ertf_Charset
{
  ansi,
  mac,
  pc,
  pca
};

struct Ertf_Document
{
  char *filename;
  FILE *stream;
  char *markup;
  int   markup_position;
  int   version;
  int   bracecount;
  enum Ertf_Charset charset;
  // todo: add a member for summary information of the document
};

Ertf_Document *
ertf_document_new(void)
{
  Ertf_Document *doc;

  doc = (Ertf_Document *)malloc(sizeof(Ertf_Document));
  if (!doc)
    return NULL;

  doc->filename = NULL;
  doc->stream = NULL;
  doc->markup = NULL;
  //  doc->charset[0] = '\0';
  doc->version = -1;
  doc->bracecount = 0;

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
      ungetc(c, doc->stream);
      return 1;
    case '\\':
      if(ertf_tag_get(doc->stream, tag))
      {
	fprintf(stderr, "ertf_document_header_get: encountered EOF while reading control tag.\n");
	return 0;
      }
      if (strcmp(tag, "rtf") == 0)
	fscanf(doc->stream, "%d", &doc->version);
      /* charset tags */
      else if (strcmp(tag, "ansi") == 0)
      {
	doc->charset = ansi;
      }
      else if (strcmp(tag, "mac") == 0)
      {
	doc->charset = mac;
      }
      else if (strcmp(tag, "pc") == 0)
      {
	doc->charset = pc;
      }
      else if (strcmp(tag, "pca") == 0)
      {
	doc->charset = pca;
      }
      /* default font */
      else if (strcmp(tag, "deff") == 0)
      {
	fscanf(doc->stream, "%d", &_ertf_default_font);      
      }
      else
      {
	fprintf(stderr, "ertf_document_header_get: unrecognised control tag %s\n", tag);
      }
      break;
    default:
      fprintf(stderr, "ertf_document_header_get: unrecognised control character %c.\n", c);
    }
  }
  fprintf(stderr, "ertf_document_header_get: encountered EOF while parsing header.\n");
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
        fprintf(stderr, "ertf_document_parse: EOF encountered.\n");
        return 0;
      }

      /* font table */
      if (strcmp(control_word, "fonttbl") == 0)
      {
	if (ertf_font_table(doc->stream))
        {	
	  printf("Successfully created font table.\n");
	  doc->bracecount--;
	}
        else
	  printf("failure in creating font table.\n");

	/* color table */
      }
      else if (strcmp(control_word, "colortbl") == 0)
      {
	if (ertf_color_table(doc->stream))
        {
	  printf("Successfully created color table.\n");
	  doc->bracecount--;
	}
        else
	  printf("failure in creating color table.\n");

	/* stylesheet */
      }
      else if (strcmp(control_word, "stylesheet") == 0)
      {
	if (ertf_stylesheet_parse(doc->stream))
        {
	  printf("Successfully created stylesheet table.\n");
	  doc->bracecount--;
	}
        else
	  printf("failure in creating stylesheet table.\n");
      }

      /* paragraph */
      else if (strcmp(control_word, "pard") == 0)
      {
	ertf_markup_add("<p>", 3);
	if (ertf_paragraph_translate(doc->stream, 0))
        {
	  printf("Successfully parsed a paragraph.\n");
	}
        else
	  printf("failure parsing parapgraph.\n");
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
	if (ertf_summary(doc->stream))
	  printf("Successfully parsed information section.\n");
	else
	  printf("failure parsing information about the file.\n");
      }

      /* unrecognised control word */
      else
      {
	fprintf(stderr, "ertf_document_parse: skipping unrecognised control word %s\n", control_word);
      }

      break;

    default:
      fprintf(stderr, "ertf_document_parse: skipped control char `%c'\n", c);
    }
  }

  markup[ertf_markup_position] = '\0';
  doc->markup = markup;
  printf("%d\nmarkup:\n%s\n", ertf_markup_position, doc->markup);
  // When end-of-file is reached, check if  parsing is complete. In case,
  // it is not, print an error message stating "incomplete rtf file".
  if (doc->bracecount)
    fprintf(stderr, "ertf_document_parse: Ill-formed rtf - inconsistent use of braces.\n");

  return 1;
}

int
ertf_document_version_get(Ertf_Document *doc)
{
  if (!doc)
    return -1;

  return doc->version;
}

/*const char *
ertf_document_charset_get(Ertf_Document *doc)
{
  if (!doc)
    return NULL;

  return doc->charset;
  }*/

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
