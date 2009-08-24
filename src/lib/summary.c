#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "summary.h"


static int _ertf_resolve_control_word(FILE *);

// todo: deal with the case of multiple \info tags

/*
 * This function is called when \info tag is encountered.
 */
int
ertf_summary(FILE *fp)
{
  int c;

  doc_info = (Ertf_Info *)malloc(sizeof(Ertf_Info));

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
    case '{':
      if (_ertf_resolve_control_word(fp))
	break;
      else
	goto error;
    case '}':return 1;
    default:
      fprintf(stderr, "ertf_summary: Ill-formatted rtf file.\n");
      goto error;
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
  char control_word[12];
  int c;

  if ((c = fgetc(fp)) == EOF || c != '\\')
  {
    fprintf(stderr, "Ill-formed rtf.\n");
    return 0;
  }
  fscanf(fp, "%[^ ]", control_word);
  // get control word
  if (feof(fp))
  {
    fprintf(stderr, "End of file reached.\n");
    return 0;
  }

  // resolve the control word
  switch (control_word[0])
  {
  case 'a':// \author
    if (strcmp(control_word + 1, "uthor") == 0)
    {
      // the keyword is actually \author
      // todo: process it
      return 1;
    }
    else
      goto skip;
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
  fprintf(stderr, "summary: skipping control tag %s\n", control_word);
  return 1;
}
