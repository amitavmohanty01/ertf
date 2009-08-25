#include <stdlib.h>

#include "input.h"

/* keep track of the buffer's current size */
int current_size=0;

/* fast implementation if strncpy, without error checking */
static void _strncpy(char *, const char *, int n);

int
ertf_markup_add(char *string, int len)
{
  char *tmp;
  int temp;

  if (ertf_markup_position + len < current_size)
  {
    _strncpy(markup + ertf_markup_position, string, len);
  }
  else
  {
    temp = ertf_markup_position + len - current_size;
    _strncpy(markup + ertf_markup_position,
	     string,
	     len - temp
	     );
    tmp = (char *)malloc(current_size + 1024);
    _strncpy(tmp, markup, current_size);
    free(markup);
    markup = tmp;
    _strncpy(markup, string + temp, current_size);
    current_size += 1024;
  }
  ertf_markup_position += len;
}

static void
_strncpy(char *src, const char *dest, int n)
{
  int i;

  for(i = 0; i < n; i++)
    src[i] = dest[i];
}
