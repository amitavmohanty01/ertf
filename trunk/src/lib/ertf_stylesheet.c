#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <Eina.h>

#include "ertf_stylesheet.h"
#include "ertf_input.h"
#include "ertf_color.h"
#include "ertf_font.h"
#include "ertf_private.h"


Eina_Array *stylesheet_table;

static int _ertf_stylesheet_add(FILE *);
static void _ertf_textblock_style_generate();

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
      _ertf_textblock_style_generate();
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
      CHECK_EOF(fp, "_ertf_stylesheet_add: Ill-formed rtf.\n", goto error);

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
	    fprintf(fp, "_ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	  style->set |= STYLE_FOREGROUND_SET;
	}
        else if (strcmp(buf + 1, "b") == 0)
        {
	  fscanf(fp, "%u", &style->background_colour);
	  if (style->background_colour >= colour_max)
          {
	    fprintf(fp, "_ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	  style->set |= STYLE_BACKGROUND_SET;
	}
	break;

      case 's':
	if (buf[1] == '\0')
        {
	  // read style number
	  fscanf(fp, "%d", &style->style_number);
	  if (feof(fp))
          {
	    fprintf(stderr, "_ertf_stylesheet_add: end of file reached while reading stylesheet number");
	    goto error;
	  }
	}
        else if (strcmp(buf + 1,"basedon") == 0)
        {
	  fscanf(fp, "%d", &c);
	  if (feof(fp))
          {
	    fprintf(stderr, "_ertf_stylesheet_add: end of file reached while reading style number");
	    goto error;
	  }
	  //todo: copy relevant parts of the style
	}
        else if (strcmp(buf + 1, "next") == 0)
        {
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "_ertf_stylesheet_add: end of file reached while reading paragraph number", goto error);
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
	    fprintf(stderr, "_ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
	    goto error;
	  }
          else if (c == '\\')
          {
	    ungetc(c, fp);
	  }
	}
	break;

      case '*':
	while((c = fgetc(fp)) != EOF && c != ';')
	  ;
	CHECK_EOF(fp, "_ertf_stylesheet_add: EOF encountered while skipping spcial style group.\n", goto error);
	ungetc(c, fp);
	break;

      case 'f':
	if (buf[1] == '\0')
        {
	  fscanf(fp, "%u", &style->font_number);
	  CHECK_EOF(fp, "_ertf_stylesheet_add: end of file reached while reading font number", goto error);
	  style->set |= STYLE_FONT_SET;
	}
        else if (strcmp(buf + 1, "s") == 0)
        {
	  fscanf(fp, "%u", &style->font_size);
	  CHECK_EOF(fp, "_ertf_stylesheet_add: end of file reached while reading font number", goto error);
	  style->set |= STYLE_FONT_SIZE_SET;
	}
        else
        {
          // unrecognised/unsupported tag
	  // todo: check if NOR conversion simplifies it
	  while ((c = fgetc(fp)) != EOF  && c != '\\')
	    ;
	  if (c == EOF)
          {
	    fprintf(stderr, "_ertf_stylesheet_add: end of file encountered while skipping unrecognised tag \n");
	    goto error;
	  }
          else if (c == '\\')
          {
	    ungetc(c, fp);
	  }
	}	
	break;

      default:// skip unrecognised/unsupported control word
	while ((c = fgetc(fp)) != EOF  && c != '\\')
	  ;
	if (c == EOF)
        {
	  fprintf(stderr, "_ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
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
      while ((c = fgetc(fp)) != EOF  && c != '}')
	;
      if(c == EOF)
      {
	fprintf(stderr, "_ertf_stylesheet_add: end of file encountered while skipping unrecognised group\n");
	goto error;
      }
      break;

    case ';':// end of style
      eina_array_push(stylesheet_table, style);
      return 1;

    case ' ':// get style name
      fscanf(fp, "%[^;]", style->name);
      CHECK_EOF(fp, "_ertf_stylesheet_add: end of file encountered while reading stylesheet name.\n", goto error);
      break;

    default:
      fprintf(stderr, "_ertf_stylesheet_add: unrecognised control character `%c'.\n", c);
      goto error;
    }
  }
  // end of file is reached
  // todo: remove debug statement in final version
  fprintf(stderr, "_ertf_stylesheet_add: Ill-formed rtf.\n");

 error:
  free(style);
  return 0;
}

char ertf_style_string[1024] = "";

static void
_ertf_textblock_style_generate()
{
  char buf[256] = "";
  Ertf_Font_Node *font;
  Eina_Array_Iterator iterator;
  unsigned int i;
  
  // todo: use functions in ertf_input.c generate style string of exact size
  font = eina_array_data_get(font_table, _ertf_default_font);  
  sprintf(buf, "DEFAULT='font=%s font_size=12 align=left color=#%02x%02x%02xff wrap=word left_margin=+12 right_margin=+12'", font->family, _ertf_default_color_r, _ertf_default_color_g, _ertf_default_color_b);
  strcat(ertf_style_string, buf);
  sprintf(buf, "bold='+ font=%s:style=Bold wrap=word font_size=12'", font->family);
  strcat(ertf_style_string, buf);
  sprintf(buf, "/bold='-'");
  strcat(ertf_style_string, buf);
  sprintf(buf, "br='\n'");
  strcat(ertf_style_string, buf);
  sprintf(buf, "tab='\t'");
  strcat(ertf_style_string, buf);
  sprintf(buf, "p='+ font=%s font_size=12 align=left wrap=word left_margin=+12 right_margin=+12'/p='- \n'", font->family);
  strcat(ertf_style_string, buf);
  sprintf(buf, "center='+ font=%s font_size=12 align=center'/center='- \n'", font->family);
  strcat(ertf_style_string, buf);
  sprintf(buf, "right='+ align=right left_margin=+12 right_margin=+12'/right='- align=left'");
  strcat(ertf_style_string, buf);

  EINA_ARRAY_ITER_NEXT(font_table, i, font, iterator)
  {
    printf("%d# %s\n", i, font->family);
  }
}

char *
ertf_textblock_style_get()
{
  return ertf_style_string;
}
