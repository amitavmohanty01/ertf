#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ertf_document.h"
#include "ertf_private.h"


struct Ertf_Document
{
  char *filename;
  FILE *stream;
  char *markup;
  char  charset[6];
  int   markup_position;
  int   version;
  int   bracecount;
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
  doc->charset[0] = '\0';
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

  if ((c = getc(doc->stream)) == EOF)
  {
    // todo:display blank textblock for empty file
  }
  else if (c != '{')
  {
    // An rtf file should start with `{\rtf'
    fprintf(stderr, "RTF file must begin with {\\rtf\n");
    return 0;
  }
  else if (fscanf(doc->stream, "%4s", str), strcmp(str, "\\rtf") != 0 )
  {
    fprintf(stderr, "rtf version unspecified.\n");
    return 0;
  }
  else if ((fscanf(doc->stream, "%d\\%c", &doc->version, &str[0]), str[0] != 'a') &&
	   str[0] != 'p' &&
	   str[0] != 'm')
  {
    // todo: improve the if condition for full word checking
    fprintf(stderr, "charset not defined\n");
    return 0;
  }
  else
  {
    //increase brace count
    doc->bracecount++;

    // store charset
    ungetc(str[0], doc->stream);
    fscanf(doc->stream, "%[^\\{]", doc->charset);
  }

  return 1;
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
      fscanf(doc->stream, "%[^ {\\;0123456789]", control_word);
      // Interestingly, a semi-colon delimits the "\colortbl" keyword sometimes

      if (feof(doc->stream))
      {
        fprintf(stderr, "readloop: EOF encountered.\n");
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

      break;

    default:
      fprintf(stderr, "readloop: skipped control char `%c'\n", c);
    }
  }

  markup[ertf_markup_position] = '\0';
  printf("%d\nmarkup:\n%s\n", ertf_markup_position, markup);
  // When end-of-file is reached, check if  parsing is complete. In case,
  // it is not, print an error message stating "incomplete rtf file".
  if (doc->bracecount)
    fprintf(stderr, "readloop: Ill-formed rtf - inconsistent use of braces.\n");

  return 1;
}

int
ertf_document_version_get(Ertf_Document *doc)
{
  if (!doc)
    return -1;

  return doc->version;
}

const char *
ertf_document_charset_get(Ertf_Document *doc)
{
  if (!doc)
    return NULL;

  return doc->charset;
}