#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <eina_array.h>

#include "ertf_stylesheet.h"
#include "ertf_input.h"
#include "ertf_color.h"
#include "ertf_private.h"


Eina_Array *stylesheet_table;

static int _ertf_stylesheet_add(FILE *);


int ertf_stylesheet_parse(FILE *fp)
{
  int c;

  // todo: remove debug msg
  printf("Inside stylesheet parser.\n");

  // create an eina array
  stylesheet_table = eina_array_new(7);
  if (!stylesheet_table)
  {
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    return 0;
  }

  while ((c = fgetc(fp)) != EOF)
  {
    switch (c)
    {
    case '{':
      if (!_ertf_stylesheet_add(fp))
      {
	fprintf(stderr, "ertf_stylesheet_parse: Ill-formed rtf.\n");
	return 0;
      }
      if ((c = fgetc(fp)) == EOF || c!='}')
      {
	// end braces for each style is consumed here.
	goto err_loop;
      }      
      break;

    case '}':// end of stylesheets
      return 1;// successful return

    case '\n':// todo: check whether newline character is a valid delimiter?
      // the spec doesn't mention so as far as I know.
      // The issue came up during debugging and I am just skipping it as of now.
      // Seeing the rtf file, that is what I interpret it to be.
      break;

    default:
      fprintf(stderr, "ertf_stylesheet_parse: invalid character `%c'.\n", c);
      return 0;
    }
  }

 err_loop:
  fprintf(stderr, "End of file reached in stylesheet table.\n");
  return 0;// unsuccessful return
}


static int
_ertf_stylesheet_add(FILE *fp)
{
  char buf[12];
  int c;
  Ertf_Stylesheet *style;
  unsigned int colour_max;

  style = (Ertf_Stylesheet *)malloc(sizeof(Ertf_Stylesheet));
  if (!style)
  {
    fprintf(stderr, "_ertf_stylesheet_add: out of memory while allocating style node.\n");
    return 0;
  }
  colour_max = eina_array_count_get(color_table);

  // todo: remove debug msg
  printf("Inside stylesheet entry parser.\n");

  while ((c = fgetc(fp)) != EOF)
  {
    switch (c)
    {
    case '\\':// get the control word
      fscanf(fp, "%[^ 0123456789\\]", buf);
      // read until a delimiter is encountered
      CHECK_EOF(fp, "ertf_stylesheet_add: Ill-formed rtf.\n", goto error);

      // todo: use pattern matching technique / hashing and benchmark them
      // to decide the optimal alternative to the if-else ladder
      switch (buf[0])
      {
      case 'c':
	if (strcmp(buf + 1,"f") == 0)
        {
	  // read the colour table entry number
	  fscanf(fp, "%u", &style->foreground_colour);
	  if (style->foreground_colour >= colour_max)
          {
	    fprintf(fp, "ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	}
        else if (strcmp(buf + 1, "b") == 0)
        {
	  fscanf(fp, "%u", &style->background_colour);
	  if (style->background_colour >= colour_max)
          {
	    fprintf(fp, "ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	}
	break;

      case 's':
	if (buf[1] == '\0')
        {
	  // read font number
	  fscanf(fp, "%d", &style->style_number);
	  if (feof(fp))
          {
	    fprintf(stderr, "ertf_stylesheet_add; end of file reached while reading stylesheet number");
	    goto error;
	  }
	}
        else if (strcmp(buf + 1,"basedon") == 0)
        {
	  fscanf(fp, "%d", &c);
	  if (feof(fp))
          {
	    fprintf(stderr, "ertf_stylesheet_add; end of file reached while reading style number");
	    goto error;
	  }
	  //todo: copy relevant parts of the style
	}
        else if (strcmp(buf + 1, "next") == 0)
        {
	  fscanf(fp, "%d", &c);
	  if (feof(fp))
          {
	    fprintf(stderr, "ertf_stylesheet_add; end of file reached while reading paragraph number");
	    goto error;
	  }
	  // todo: set relevant paragraphs for the style
	}
        else
        {
          // unrecognised/unsupported tag
	  // todo: check if NOR conversion simplifies it
	  while ((c = fgetc(fp)) != EOF  && c != '\\')
	    ;
	  if (c == EOF)
          {
	    fprintf(stderr, "ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
	    goto error;
	  }
          else if (c == '\\')
          {
	    ungetc(c, fp);
	  }
	}
	break;

      case 'f':
	if (buf[1] == '\0')
        {
	  fscanf(fp, "%u", &style->font_number);
	  if (feof(fp))
          {
	    fprintf(stderr, "ertf_stylesheet_add; end of file reached while reading font number");
	    goto error;
	  }
	}
        else if (strcmp(buf + 1, "s") == 0)
        {
	  fscanf(fp, "%u", &style->font_size);
	  if (feof(fp))
          {
	    fprintf(stderr, "ertf_stylesheet_add; end of file reached while reading font number");
	    goto error;
	  }
	}
        else
        {
          // unrecognised/unsupported tag
	  // todo: check if NOR conversion simplifies it
	  while ((c = fgetc(fp)) != EOF  && c != '\\')
	    ;
	  if (c == EOF)
          {
	    fprintf(stderr, "ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
	    goto error;
	  }
          else if (c == '\\')
          {
	    ungetc(c, fp);
	  }
	}	
	break;

      default:// skip unrecognised/unsupported control word
	// todo: check if NOR conversion simplifies it
	while ((c = fgetc(fp)) != EOF  && c != '\\')
	  ;
	if (c == EOF)
        {
	  fprintf(stderr, "ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
	  goto error;
	}
        else if (c == '\\')
        {
	  ungetc(c, fp);
	}
      }
      break;

      // parse group
    case '{':
      // todo: check if NOR conversion simplifies it
      while ((c = fgetc(fp)) != EOF  && c != '}')
	;
      if(c == EOF)
      {
	fprintf(stderr, "ertf_stylesheet_add: end of file encountered while skipping unrecognised group\n");
	goto error;
      }
      break;

    case ';':// end of style
      eina_array_push(stylesheet_table, style);
      return 1;

    case ' ':// get style name
      fscanf(fp, "%[^;]", style->name);
      if (feof(fp))
      {
	fprintf(stderr, "ertf_stylesheet_add: end of file encountered while reading stylesheet name.\n");
	goto error;
      }
      break;

    default:
      fprintf(stderr, "ertf_stylesheet_add: unrecognised control character `%c'.\n", c);
      goto error;
    }
  }
  // end of file is reached
  // todo: remove debug statement in final version
  fprintf(stderr, "ertf_font_add: Ill-formed rtf.\n");

 error:
  free(style);
  return 0;
}
