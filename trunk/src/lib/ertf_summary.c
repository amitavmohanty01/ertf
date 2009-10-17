#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ertf_summary.h"
#include "ertf_input.h"


Ertf_Info *doc_info;

static int _ertf_resolve_control_word(FILE *);

/*
 * This function is called when \info tag is encountered.
 */
int
ertf_summary(FILE *fp)
{
  int c;
  int braces = 1;
  doc_info = (Ertf_Info *)malloc(sizeof(Ertf_Info));
  if (!doc_info)
  {
    fprintf(stderr, "ertf_summary: out of memory while allocating doc_info.\n");
    return 0;
  }

  // default values
  doc_info->author = NULL;
  doc_info->title = NULL;
  doc_info->subject = NULL;
  doc_info->operator = NULL;
  doc_info->keywords = NULL;
  doc_info->comment = NULL;
  doc_info->version = 0;// zero indicates not set
  doc_info->doccomm = NULL;
  doc_info->internal_version = 0;
  doc_info->pages = -1;
  doc_info->words = -1;
  doc_info->chars = -1;
  doc_info->internal_ID = 0;
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
      printf("%d\t", braces);
      if (_ertf_resolve_control_word(fp))
	break;
      else
	goto error;
    case '}':
      braces--;
      printf("%d\n", braces);
      if (braces == 0)
	return 1;
      break;
    default:
      fprintf(stderr, "ertf_summary: skipping control character %c\n", c);      
    }
  }

  // free it in case of EOF and in presence of invalid characters
 error:
  free(doc_info);
  return 0;
}
/*
 * This function reads each summary group till the end brace (including it).
 */
static int
_ertf_resolve_control_word(FILE *fp)
{
  // keeping it a multiple of four
  char control_word[20];
  int c;
  if ((c = fgetc(fp)) == EOF || c != '\\')
  {
    fprintf(stderr, "_ertf_resolve_control_word: Ill-formed rtf.\n");
    return 0;
  }
  fscanf(fp, "%[^ \\0123456789{}]", control_word);
  // get control word
  CHECK_EOF(fp, "_ertf_resolve_control_word: End of file reached while reading control word.\n", return 0);

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
      doc_info->author = s;
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
      fscanf(fp, "%d", &doc_info->version);
      CHECK_EOF(fp, "_ertf_resolve_control_word: EOF encountered while reading version.\n", return 0);
      return 1;
    }
    else if (strcmp(control_word + 1, "ern") == 0)
    {
      fscanf(fp, "%d", &doc_info->internal_version);
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
