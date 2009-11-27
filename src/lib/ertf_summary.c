#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ertf_document.h"
#include "ertf_summary.h"
#include "ertf_input.h"
#include "ertf_private.h"


static int _ertf_resolve_control_word(Ertf_Document *doc);

/*
 * This function is called when \info tag is encountered.
 */
int
ertf_summary(Ertf_Document *doc)
{
  int c;
  int braces = 1;
  FILE *fp;

  /* doc is never NULL when this function is called */

  doc->summary = (Ertf_Summary *)malloc(sizeof(Ertf_Summary));
  if (!doc->summary)
  {
    fprintf(stderr, "ertf_summary: out of memory while allocating doc_info.\n");
    return 0;
  }

  fp = doc->stream;

  // default values
  doc->summary->author = NULL;
  doc->summary->title = NULL;
  doc->summary->subject = NULL;
  doc->summary->operator = NULL;
  doc->summary->keywords = NULL;
  doc->summary->comment = NULL;
  doc->summary->version = 0;// zero indicates not set
  doc->summary->doccomm = NULL;
  doc->summary->internal_version = 0;
  doc->summary->pages = -1;
  doc->summary->words = -1;
  doc->summary->chars = -1;
  doc->summary->internal_ID = 0;
  // todo: update for default of time variables

  // obtain keywords till the end of group with proper error checking
  while ((c = fgetc(fp)) != EOF)
  {
    switch(c)
    {
    case '\\':ungetc(c, fp);
    case '{':
      if (c == '{')
	braces++;      
      if (_ertf_resolve_control_word(doc))
	break;
      else
	goto error;
    case '}':
      braces--;
      if (braces == 0)
	return 1;
      break;
    default:
      fprintf(stderr, "ertf_summary: skipping control character %c\n", c);      
    }
  }

  // free it in case of EOF and in presence of invalid characters
 error:
  free(doc->summary);
  doc->summary = NULL;
  return 0;
}
/*
 * This function reads each summary group till the end brace (including it).
 */
static int
_ertf_resolve_control_word(Ertf_Document *doc)
{
  // keeping it a multiple of four
  char control_word[20];
  int c;
  FILE *fp;

  fp = doc->stream;

  if ((c = fgetc(fp)) == EOF || c != '\\')
  {
    fprintf(stderr, "_ertf_resolve_control_word: Ill-formed rtf.\n");
    return 0;
  }
  // get control word
  if (ertf_tag_get(fp, control_word))
  {
    fprintf(stderr, "_ertf_resolve_control_word: End of file reached while reading control word.\n");
    return 0;
  }

  // resolve the control word
  switch (control_word[0])
  {
  case 'a':// \author
    if (strcmp(control_word + 1, "uthor") == 0)
    {
      char *s;
      s = (char *)malloc(256);
      fscanf(fp, "%[^}]", s);
      CHECK_EOF(fp, "_ertf_resolve_control_word: EOF encountered while reading author name.\n", return 0);
      doc->summary->author = s;
      return 1;
    }
    else
      goto skip;
    break;
  case 'b':// \buptime
  case 'c':// \creatim, \comment
  case 'd':// \doccomm, \dy
  case 'e':// \edmins
  case 'h':// \hr
  case 'i':// \id
  case 'k':// \keywords
  case 'm':// \mo, \\min
  case 'n':// \nofpages, \nofwords, \nofchars
  case 'o':// \operator
  case 'p':// \printim
  case 'r':// \revtim
  case 's':// \subject
  case 't':// \title
  case 'v':// \version, \vern
    if (strcmp(control_word + 1, "ersion") == 0)
    {
      fscanf(fp, "%d", &doc->summary->version);
      CHECK_EOF(fp, "_ertf_resolve_control_word: EOF encountered while reading version.\n", return 0);
      return 1;
    }
    else if (strcmp(control_word + 1, "ern") == 0)
    {
      fscanf(fp, "%d", &doc->summary->internal_version);
      CHECK_EOF(fp, "_ertf_resolve_control_word: EOF encountered while reading internal version number.\n", return 0);
      return 1;
    }
    else
      goto skip;
  break;
  case 'y':// \yr
    if (strcmp(control_word + 1, "r") == 0)
    {
      // todo: process
      return 1;
    }
    else
    {
      goto skip;
    }
    //default: skip invaid tag
  }

 skip:
  fprintf(stderr, "_ertf_resolve_control_word: skipping control tag %s\n", control_word);
  return 1;
}
