#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "ertf_document.h"
#include "ertf_input.h"
#include "ertf_private.h"


/* The indicator to current location shall be maintained by the accessing*/
/* programs. */

int
ertf_tag_get(FILE *fp, char *s)
{
  int c;
  fscanf(fp, "%[^ 0123456789;\\{}\n\r]", s);
  if ((c = fgetc(fp)) != ' ')
    ungetc(c, fp);
  CHECK_EOF(fp, "EOF encountered while obtaining control tag.\n", return 1);
  return 0;
}

int
ertf_group_skip(FILE *fp)
{
  int c, brace = 0;
  while((c = fgetc(fp)) != EOF)
  {
    if (c == '{')
      brace++;
    else if (c == '}')
    {
      if (brace != 0)
	brace--;
      else
	return 0; // success
    }
    else
      ; // skip
  }
  return 1; // error
}
