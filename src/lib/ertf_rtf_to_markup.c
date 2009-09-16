#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ertf_input.h"
#include "ertf_color.h"
#include "ertf_private.h"


// textblock uses the style set by default

// \dn<N> translates to ?

// \scaps translates to ?

// \uld translates to ?

int
ertf_paragraph_translate(FILE *fp, int align)
{
  char buf[12];
  int c;

  char fontset = 0;
  char foregroundset = 0;
  char backgroundset = 0;
  char strikethroughset = 0;
  unsigned char current_r = _ertf_default_color_r;
  unsigned char current_g = _ertf_default_color_g;
  unsigned char current_b = _ertf_default_color_b;

  while((c = fgetc(fp)) != EOF)
  {
    switch(c)
    {
      /* get control word */
    case '\\':
      fscanf(fp, "%[^ 0123456789\\{}\n]", buf);
      CHECK_EOF(fp, "ertf_paragraph_translate: end-of-file encountered while "
		"retrieving control word.\n", return 0);

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
	// todo: ensure that </p> is defined in style string
	ertf_markup_add("</p>", 4);
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
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for italicisation.\n", return 0);
	// todo: find relevant markup
      }

      /* bold text */
      else if (strcmp(buf, "b") == 0)
      {
	if (!isdigit(c = fgetc(fp)))
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for bold text.\n", return 0);
	// todo: find relevant markup
      }

      /* font entry number */
      else if (strcmp(buf, "f") == 0)
      {
	if (!isdigit(c = fgetc(fp)))
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while checking for bold text.\n", return 0);
	// todo: find relevant markup
      }

      /* underline */
      else if (strcmp(buf, "ul") == 0)
      {
	char temp[6];
	char color[10];
	Ertf_Color *node;
	
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

	if (!ertf_paragraph_translate(fp, align))
	  return 0;
	ertf_markup_add("</>", 3);
      }

      /* double underline */
      else if (strcmp(buf, "uldb") == 0)
      {
	ertf_markup_add("<underline=double>", 18);
	if (!ertf_paragraph_translate(fp, align))
	  return 0;
	ertf_markup_add("</>", 3);
      }

      /* font size */
      else if (strcmp(buf, "fs") == 0)
      {
	if (fontset == 0)
	{
	ertf_markup_add("<font_size=", 11);
	while (isdigit(c = fgetc(fp)))
	{
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF encountered while getting font size.\n", return 0);
	  ertf_markup_add(&c, 1);
	}
	ungetc(c, fp);
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

	/* read till next delimiter */
	while ((c = fgetc(fp)) != '\\' &&
	        c != ' ' &&
	        c != '{' &&
	        c != '}')
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF reached while skipping"
		    " control info.\n", return 0);
	ungetc(c, fp);
      }
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
	//ertf_markup_add("<br>", 4);
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

  return 1;
}
