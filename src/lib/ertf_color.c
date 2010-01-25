#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <Eina.h>

#include "ertf_document.h"
#include "ertf_color.h"
#include "ertf_input.h"
#include "ertf_private.h"


Eina_Array *color_table = NULL;

static int _ertf_color_add(FILE *);
static void _ertf_color_generate_markup(void);

/*
 * This function creates a color table for an rtf file. It returns 1 upon 
 * success and 0 in case of failure.
 */
int
ertf_color_table(FILE *fp)
{
  int c;

  DBG("Inside color table handler.\n");

  // create an eina array
  color_table = eina_array_new(7);
  if (!color_table)
  {
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    return 0;
  }

  // ready to parse the color table now
  while ((c=fgetc(fp)) != EOF)
  {
    switch (c)
    {
      Ertf_Color *node;

    case ';':// indicates default color
      node = (Ertf_Color *)malloc(sizeof(Ertf_Color));
      if (!node)
      {
	ERR("short of memory while allocating color node");
	return 0;
      }
      node->r = _ertf_default_color_r;
      node->g = _ertf_default_color_g;
      node->b = _ertf_default_color_b;
      eina_array_push(color_table, node);
      break;

    case '\\':
      ungetc(c, fp);
      if (_ertf_color_add(fp))
	continue;
      else
	INFO("Ill-formed rtf");
      break;

    case '}':// end of color table
      // generate markup strings for all colors
      _ertf_color_generate_markup();
      return 1;

    default:
      DBG("skipping control character %c.", c);      
    }
  }

  ERR("End of file reached in color table.");
  return 0;
}

/*
 * This function adds an entry to the color table. It returns 1 on success and
 * 0 in case of failure.
 */

#define RED 1
#define GREEN 2
#define BLUE 4

static int
_ertf_color_add(FILE *fp)
{
  char color[7];
  int index;
  int set = 0;
  Ertf_Color *node;
  int c;

  node = (Ertf_Color *)malloc(sizeof(Ertf_Color));
  if (!node)
  {
    ERR("short of memory while allocating color node");
    return 0;
  }

  DBG("Inside color entry parser");

  while ((c = fgetc(fp)) != EOF)
  {
    switch (c)
    {
    case '\\':      
      if (ertf_tag_get(fp, color))
      {
	ERR("EOF encountered while getting control tag");
	goto err;
      }

      fscanf(fp, "%d", &index);// todo: do error checking for range of rgb value
      if (feof(fp)) goto err;

      if (strcmp(color, "green") == 0)
      {
	if (!(set & GREEN))
	  node->g = index;
	else
	  WARN("multiple values for same color");
      }
      else if (strcmp(color, "red") == 0)
	node->r = index;
      // todo: add set/unset check as above
      else if (strcmp(color, "blue") == 0)
	node->b = index;
      // todo: add set/unset check as above
      else
	// continue as the tag is not recognised and should be therefore skipped
	DBG("skipped unrecognised control tag `%s'", color);
      break;
    case ';':// todo: error checking on success of the push operation
      eina_array_push(color_table, node);
      return 1;
    default:
      DBG("skipped control character %c.\n", c);      
    }    
  }

 err:
  free(node);
  return 0;
}

static void
_ertf_color_generate_markup(void)
{
  Eina_Array_Iterator iterator;
  unsigned int i;
  Ertf_Color *node;

  EINA_ARRAY_ITER_NEXT(color_table, i, node, iterator){
    sprintf(node->string, "%02x%02x%02xff", node->r, node->g, node->b);
  }
}
