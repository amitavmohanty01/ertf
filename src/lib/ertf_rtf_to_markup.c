#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ertf_document.h"
#include "ertf_input.h"
#include "ertf_color.h"
#include "ertf_font.h"
#include "ertf_private.h"

int
ertf_paragraph_translate(FILE *fp, int align)
{
  char buf[12];
  int c;

  char fontset = 0;
  char foregroundset = 0;
  char backgroundset = 0;
  char strikethroughset = 0;
  char underlineset = 0;
  char boldset = 0;
  char italicset = 0;
  unsigned char current_r = _ertf_default_color_r;
  unsigned char current_g = _ertf_default_color_g;
  unsigned char current_b = _ertf_default_color_b;

  while((c = fgetc(fp)) != EOF)
  {
    switch(c)
    {
      /* get control word */
    case '\\':
      /* handle special characters */
      if ((c = getc(fp)) == '\\' || c == '{' || c == '}' || c == '\'')
      {
	if (c == '\'')
	{
	  fscanf(fp, "%x", &c);
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while reading hexadecimal value of character.\n", return 0);	 
	}
	ertf_markup_add(&c, 1);
	break;
      }
      else
	ungetc(c, fp);
      
      fscanf(fp, "%[^ 0123456789;\\{}\n\r]", buf);
      CHECK_EOF(fp, "ertf_paragraph_translate: end-of-file encountered while retrieving control word.\n", return 0);

      /* reset to default character formatting */
      if (strcmp(buf, "plain") == 0)
      {
      }

      /* get the style number */
      else if (strcmp(buf, "s") == 0)
      {
	// todo: add the relevant style string to markup
	fgetc(fp);
	CHECK_EOF(fp, "ertf_paragraph_translate: end-of-file encountered while "
		"retrieving style number.\n", return 0);
      }

      /* right aligned text */
      else if (strcmp(buf, "rtlch") == 0)
      {
	// todo: either insert align=right in markup or in a style
	ertf_markup_add("<right>", 7);

	if (!ertf_paragraph_translate(fp, 1))
	  return 0;
	ertf_markup_add("</right>", 8);
      }

      /* end of paragraph */
      else if (strcmp(buf, "par") == 0)
      {
	int c;
	// todo: ensure that </p> is defined in style string
	ertf_markup_add("</p>", 4);
	// read till end of group
	while ((c = fgetc(fp)) != EOF && c != '{' && c != '\\')
	  ;
	if (feof(fp))
	  return 0;
	else
	  ungetc(c, fp);
	goto success;
      }

      /* left aligned text */
      else if (strcmp(buf, "ltrch") == 0 && align == 1)
      {
	goto success;
      }

      /* italicized text */
      else if (strcmp(buf, "i") == 0) 
      {
	if (!isdigit(c = fgetc(fp)))
	{
	  ungetc(c, fp);
	  if (!italicset)
	  {
	    ertf_markup_add("<em>", 4);
	    italicset++;
	  }
	}
	else
	{
	  if (italicset)
	  {
	    ertf_markup_add("</em>", 5);
	    italicset = 0;
	  }
	}
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for italicisation.\n", return 0);	
      }

      /* bold text */
      else if (strcmp(buf, "b") == 0)
      {
	if (!isdigit(c = fgetc(fp)))
	{
	  ungetc(c, fp);
	  if (!boldset)
	  {
	    ertf_markup_add("<bold>", 6);
	    boldset++;
	  }
	}
	else
	{
	  if (boldset)
	  {
	    ertf_markup_add("</bold>", 7);
	    boldset = 0;
	  }
	}
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for bold text.\n", return 0);
      }

      /* font entry number */
      else if (strcmp(buf, "f") == 0)
      {
	Ertf_Font_Node *font;	

	// todo: find relevant markup
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for font number.\n", return 0);
	font = eina_array_data_get(font_table, c);	
      }

      /* line break */
      else if (strcmp(buf, "line") == 0)
      {
	ertf_markup_add("<br>", 4);
      }

      /* tab */
      else if (strcmp(buf, "tab") == 0)
      {
	ertf_markup_add("<tab>", 5);
      }

      /* underline */
      else if (strcmp(buf, "ul") == 0)
      {
	char temp[6];
	char color[10];
	Ertf_Color *node;

	if (underlineset)
	{
	  ertf_markup_add("</>", 3);
	}
	if ((c = getc(fp)) == 0)
	{
	  underlineset = 0;	  
	}
	else
	{
	  ungetc(c, fp);
	  fscanf(fp, "%4s", temp);	
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for underline colour.\n", return 0);
	  if (strcmp(temp, "\\ulc") == 0)
	  {
	    fscanf(fp, "%d", &c);
	    CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for colour number for underlining.\n", return 0);
	    node = (Ertf_Color *)eina_array_data_get(color_table, c);
	    ertf_markup_add("<underline=on underline_color=#", 31);
	    ertf_markup_add(node->string, 8);
	    ertf_markup_add(">", 1);	  
	  }
	  else
	  {
	    ertf_markup_add("<underline=on underline_color=#", 31);
	    sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	    ertf_markup_add(color, 8);
	    ertf_markup_add(">", 1);
	  }

	  underlineset++;	  
	}
      }

      /* double underline */
      else if (strcmp(buf, "uldb") == 0)
      {
	char temp[6];
	char color[10];
	Ertf_Color *node;

	if (underlineset)
	{
	  ertf_markup_add("</>", 3);
	}
	fscanf(fp, "%4s", temp);	
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for underline colour.\n", return 0);
	if (strcmp(temp, "\\ulc") == 0)
	{
	    fscanf(fp, "%d", &c);
	    CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for colour number for underlining.\n", return 0);
	    node = (Ertf_Color *)eina_array_data_get(color_table, c);
	    ertf_markup_add("<underline=double underline_color=#", 35);
	    ertf_markup_add(node->string, 8);
	    ertf_markup_add(" underline2_color=#", 19);
	    ertf_markup_add(node->string, 8);
	    ertf_markup_add(">", 1);	  
	}
	else
	{
	    ertf_markup_add("<underline=double underline_color=#", 35);
	    sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	    ertf_markup_add(color, 8);
	    ertf_markup_add(" underline2color=#", 19);
	    ertf_markup_add(color, 8);
	    ertf_markup_add(">", 1);
	}

	underlineset++;	
      }

      /* font size */
      else if (strcmp(buf, "fs") == 0)
      {
	char store[10];	

	if (fontset == 0)
	{
	  ertf_markup_add("<font_size=", 11);
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while getting font size.\n", return 0);
	  /* integer divison yields the floor value, which is used as the font size as fractional font size is currently not supported */
	  // todo: implement support for fractional font size in text block.
	  c /= 2;
	  sprintf(store, "%d", c);
	  ertf_markup_add(store, strlen(store));
	  ertf_markup_add(">", 1);
	  fontset = 1;
	}
	else 
	{
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while skipping font size.\n", return 0);
	}
      }

      /* handle foreground colour */
      else if (strcmp(buf, "cf") == 0)
      {
	Ertf_Color *node;
	if (foregroundset)
	{
	  ertf_markup_add("</>", 3);
	}
	ertf_markup_add("<color=#", 8);
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while reading colour number.\n", return 0);
	node = (Ertf_Color *) eina_array_data_get(color_table, c);
	if(!node){
	  printf("invalid node.");
	  exit(1);
	}
	ertf_markup_add(node->string, 8);
	ertf_markup_add(">", 1);
	foregroundset++;
	current_r = node->r;
	current_g = node->g;
	current_b = node->b;
      }

      /* handle background colour */
      else if (strcmp(buf, "cb") == 0)
      {
	Ertf_Color *node;	
	if (backgroundset)
	{
	  ertf_markup_add("</>", 3);
	}
	ertf_markup_add("<backing=on backing_color=#", 27);
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while reading background colour number.\n", return 0);
	node = (Ertf_Color *)eina_array_data_get(color_table, c);
	ertf_markup_add(node->string, 8);
	ertf_markup_add(">", 1);
	backgroundset++;
      }

      /* handle strikethrough */
      else if (strcmp(buf, "strike") == 0)
      {
	if (!strikethroughset)
	{
	  char color[9];
	  ertf_markup_add("<strikethrough=on strikethrough_color=#", 39);
	  sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	  ertf_markup_add(color, 8);
	  ertf_markup_add(">", 1);
	  strikethroughset++;
	}
      }

      /* handle target */
      else if (strcmp(buf, "*") == 0)
      {
	/* They not supported in the prototype. */
	while ((c = fgetc(fp)) != EOF && c != '}')
	  ;
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF reached while handling unsupported target.\n", return 0);
	ungetc(c, fp);
      }

      /* unsupported or unrecognised control tag */
      else
      {
	fprintf(stderr, "ertf_paragraph_translate: skipped control tag `%s'\n", buf);
      }
      /* read till next delimiter */
      while ((c = fgetc(fp)) != '\\' &&
	     c != ' ' &&
	     c != '{' &&
	     c != '}')
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF reached while skipping"
		  " control info.\n", return 0);
      if (c != ' ')
      ungetc(c, fp);

      break;

      /* handle group */
    case '{':
      if (!ertf_paragraph_translate(fp, align))
	return 0;
      break;

      /* end of group */
    case '}':
	goto success;

    default:
      if (c == '\n')
      {
	/* Since unnecessary newlines are added by rtf writers, they are ignored. The tag <br> is used when \line is encountered. */
      }
      else
      {
	ertf_markup_add(&c, 1);
      }
    }
  }
  fprintf(stderr, "ertf_paragraph_translate: EOF encountered while looping for"
	  " control word.\n");
  return 0;

 success:
  if (boldset)
  {
    ertf_markup_add("</bold>", 7);
  }
  if (italicset)
  {
    ertf_markup_add("</em>", 5);
  }
  if (fontset)
  {
    ertf_markup_add("</>", 3);
  }
  if (foregroundset)
  {
    ertf_markup_add("</>", 3);
  }
  if (backgroundset)
  {
    ertf_markup_add("</>", 3);
  }
  if (strikethroughset)
  {
    ertf_markup_add("</>", 3);
  }
  if (underlineset)
  {
    ertf_markup_add("</>", 3);
  }

  return 1;
}
