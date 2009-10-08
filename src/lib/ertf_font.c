#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <Eina.h>

#include "ertf_font.h"
#include "ertf_input.h"
#include "ertf_private.h"


Eina_Array *font_table = NULL;

static int _ertf_font_add(FILE *);


/*
 * This function creates a colour table for an rtf file. It returns 1 upon 
 * success and 0 in case of failure.
 */
int ertf_font_table(FILE *fp)
{
  int c;

  // todo: remove debug msg
  printf("Inside font table handler.\n");

  // create an eina array
  font_table = eina_array_new(7);
  if (!font_table)
  {
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    // todo: check for the same before final release
    return 0;
  }

  while ((c = fgetc(fp)) != EOF)
  {
    switch (c)
    {
    case '{':// indicates more than one fonts likely
      if (!_ertf_font_add(fp)) goto err_loop;

      if ((c = fgetc(fp)) == EOF && c != '}')
      {
	// end braces for each font enumeration is consumed here.
	goto err_loop;
      }      
      break;

    case '}':// end of font table group; time to return
      return 1;// successful return

    case '\\':
      ungetc(c, fp);
      if (!_ertf_font_add(fp)) goto err_loop;
      break;

    default: goto err_loop;
    }
  }

 err_loop:
  fprintf(stderr, "ertf_font_table: Incorrect termination of file. Probably corrupted.\n");
  return 0;// unsuccessful return
}

// todo: check for multiple occurence of same tag in one entry
static int
_ertf_font_add(FILE *fp)
{
  char buf[1000];
  int c;
  Ertf_Font_Node *node;

  node = (Ertf_Font_Node *)malloc(sizeof(Ertf_Font_Node));

  if (!node)
    fprintf(stderr, "_ertf_font_add: Out of memory while allocating font node.\n");

  node->status = 0;
  // todo: remove debug msg
  printf("Inside font entry parser.\n");

  while ((c = fgetc(fp)) != EOF)
  {
    switch (c)
    {
    case '\\': //encountered a control word      
      fscanf(fp, "%[^ 0123456789\\]", buf);
      CHECK_EOF(fp, "_ertf_fond_add: Ill-formed rtf.\n", goto error);

      if (strcmp(buf, "f") == 0)
      {
	fscanf(fp, "%d", &node->number);
	CHECK_EOF(fp, "_ertf_font_add: EOF encountered while reading font number.\n", return 0);
      }
      else if (strcmp(buf, "froman") == 0)
      {
	strcpy(node->family, "Roman");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "fswiss") == 0)
      {
	strcpy(node->family, "swiss");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "fmodern") == 0)
      {
	strcpy(node->family, "modern");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "fscript") == 0)
      {
	strcpy(node->family, "script");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "fdecor") == 0)
      {
	strcpy(node->family, "decor");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "ftech") == 0)
      {
	strcpy(node->family, "tech");
	node->status |= FAMILY_SET;
      }
      else if (strcmp(buf, "fnil") == 0)
      {
	strcpy(node->family, "default");
	// todo: after the multiple occurence check, the font family checks can
	// be modified to be done only once and rather have a bitwise check
	// run each time
      }
      else if (strcmp(buf, "fcharset") == 0)
      {
	fscanf(fp, "%d", &node->charset);
	CHECK_EOF(fp, "_ertf_font_add: EOF encountered while reading charset.", return 0);
	node->status |= CHARSET_SET;
      }
      else
      {
        // skip unrecognised or unsupported tag
	while ((c = fgetc(fp)) != EOF  && c != '\\' && !isdigit(c))
	  ;
	CHECK_EOF(fp, "_ertf_font_add: end of file encountered while skipping unrecognised tag\n", goto error);
        if (c == '\\')
        {
	  ungetc(c, fp);
	}
      }
      break;

    case ' ':fscanf(fp, "%[^;{]", node->name);
      // font tables can also contains groups, especially specifying alternative fonts
      CHECK_EOF(fp, "_ertf_font_add: end of file encountered while reading font name. \n", goto error);
      break;

    case ';':// end of font entry
      eina_array_push(font_table, node);
      return 1;// successful return

    case '{':// group
      while((c = fgetc(fp)) != '}')
	;
      break;

    default:
      // todo: remove debug statement in final version
      fprintf(stderr, "_ertf_font_add: unrecognised control character '%c'.\n", c);
      goto error;
    }
  }
  // end of file is reached
  // todo: remove debug statement in final version
  fprintf(stderr, "_ertf_font_add: Ill-formed rtf.\n");

 error:
  free(node);
  return 0;
}
