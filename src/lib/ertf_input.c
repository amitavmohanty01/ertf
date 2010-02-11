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
int ertf_markup_position;
char *markup;

/* keep track of the buffer's current size */
int current_size = 0;

int
ertf_markup_add(char *string, int len)
{
  char *tmp;
  int temp;

  if (ertf_markup_position + len < current_size)
  {
    memcpy(markup + ertf_markup_position, string, len);
  }
  else
  {
    temp = ertf_markup_position + len - current_size;
    memcpy(markup + ertf_markup_position,
	     string,
	     len - temp
	     );
    tmp = (char *)malloc(current_size + 1024);
    if (!tmp)
    {
      ERR("Out of memory while allocating markup");
      return 0;
    }
    memcpy(tmp, markup, current_size);
    if (markup)
    {
      free(markup);
    }
    markup = tmp;
    memcpy(markup + current_size, string + len - temp, temp);
    current_size += 1024;
  }
  ertf_markup_position += len;
  return 1;
}

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
