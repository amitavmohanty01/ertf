#include <stdlib.h>
#include <string.h>

#include "input.h"

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
      fprintf(stderr, "ertf_markup_add: Out of memory while allocating markup.\n");
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
