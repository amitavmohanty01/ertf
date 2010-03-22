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
    case '\\':
      if ((c = getc(fp)) == '\\' || c == '{' || c == '}' || c == '\'')
      {
	/* handle 8-bit characters */
	if (c == '\'')
	{
	  char w;
	  fscanf(fp, "%x", &c);
	  CHECK_EOF(fp, "EOF encountered while reading hexadecimal value of character", return 0);
	  w = 0xc2;
	  eina_strbuf_append_char(markup_buf, w);
	}
	eina_strbuf_append_char(markup_buf, c);
	break;
      }
      else if (c == '\r' || c == '\n')
      {
	eina_strbuf_append(markup_buf, "</p>");
	//goto success;
      }
      else
	ungetc(c, fp);
     
      /* get control word */ 
      fscanf(fp, "%[^ 0123456789;\\{}\n\r]", buf);
      CHECK_EOF(fp, "EOF encountered while retrieving control word", return 0);

      /* reset to default character formatting */
      if (strcmp(buf, "plain") == 0)
      {
      }

      /* get the style number */
      else if (strcmp(buf, "s") == 0)
      {
	// todo: add the relevant style string to markup
	fgetc(fp);
	CHECK_EOF(fp, "end-of-file encountered while "
		"retrieving style number", return 0);
      }

      /* right aligned text */
      else if (strcmp(buf, "qr") == 0)
      {
	eina_strbuf_append(markup_buf, "<right>");

	if (!ertf_paragraph_translate(fp, 1))
	  return 0;
	eina_strbuf_append(markup_buf, "</right>");
      }

      /* end of paragraph */
      else if (strcmp(buf, "par") == 0)
      {
	// int c;
	eina_strbuf_append(markup_buf, "</p>");
	/*while ((c = fgetc(fp)) != EOF && c != '{' && c != '\\')
	  ;
	if (feof(fp))
	  return 0;
	else
	ungetc(c, fp);*/
	//goto success;
      }

      /* left aligned text */
      else if (strcmp(buf, "ql") == 0 && align == 1)
      {
	goto success;
      }

      /* centre aligned text */
      else if (strcmp(buf, "qc") == 0)
      {
      }

      /* justified text */
      else if (strcmp(buf, "qj") == 0)
      {
      }

      /* italicized text */
      else if (strcmp(buf, "i") == 0) 
      {
	if (!isdigit(c = fgetc(fp)))
	{
	  ungetc(c, fp);
	  if (!italicset)
	  {
	    eina_strbuf_append(markup_buf, "<em>");
	    italicset++;
	  }
	}
	else
	{
	  if (italicset)
	  {
	    eina_strbuf_append(markup_buf, "</em>");
	    italicset = 0;
	  }
	}
	CHECK_EOF(fp, "EOF encountered while checking for italicisation", return 0);	
      }

      /* bold text */
      else if (strcmp(buf, "b") == 0)
      {
	if (!isdigit(c = fgetc(fp)))
	{
	  ungetc(c, fp);
	  if (!boldset)
	  {
	    eina_strbuf_append(markup_buf, "<bold>");
	    boldset++;
	  }
	}
	else
	{
	  if (boldset)
	  {
	    eina_strbuf_append(markup_buf, "</bold>");
	    boldset = 0;
	  }
	}
	CHECK_EOF(fp, "EOF encountered while checking for bold text", return 0);
      }

      /* font entry number */
      else if (strcmp(buf, "f") == 0)
      {
	Ertf_Font_Node *font;	

	// todo: find relevant markup
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "EOF encountered while checking for font number", return 0);
	font = eina_array_data_get(font_table, c);	
      }

      /* line break */
      else if (strcmp(buf, "line") == 0)
      {
	eina_strbuf_append(markup_buf, "<br>");
      }

      /* tab */
      else if (strcmp(buf, "tab") == 0)
      {
	eina_strbuf_append(markup_buf, "<tab>");
      }

      /* underline */
      else if (strcmp(buf, "ul") == 0)
      {
	char temp[6];
	char color[10];
	Ertf_Color *node;

	if (underlineset)
	{
	  eina_strbuf_append(markup_buf, "</>");
	}
	if ((c = getc(fp)) == 0)
	{
	  underlineset = 0;	  
	}
	else
	{
	  ungetc(c, fp);
	  fscanf(fp, "%4s", temp);	
	  CHECK_EOF(fp, "EOF encountered while checking for underline colour", return 0);
	  if (strcmp(temp, "\\ulc") == 0)
	  {
	    fscanf(fp, "%d", &c);
	    CHECK_EOF(fp, "EOF encountered while checking for colour number for underlining", return 0);
	    node = (Ertf_Color *)eina_array_data_get(color_table, c);
	    eina_strbuf_append(markup_buf, "<underline=on underline_color=#");
	    eina_strbuf_append(markup_buf, node->string);
	    eina_strbuf_append_char(markup_buf, '>');
	  }
	  else
	  {
	    eina_strbuf_append(markup_buf, "<underline=on underline_color=#");
	    sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	    eina_strbuf_append(markup_buf, color);
	    eina_strbuf_append_char(markup_buf, '>');
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
	  eina_strbuf_append(markup_buf, "</>");
	}
	fscanf(fp, "%4s", temp);	
	CHECK_EOF(fp, "EOF encountered while checking for underline colour", return 0);
	if (strcmp(temp, "\\ulc") == 0)
	{
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "EOF encountered while checking for colour number for underlining", return 0);
	  node = (Ertf_Color *)eina_array_data_get(color_table, c);
	  eina_strbuf_append(markup_buf, "<underline=double underline_color=#");
	  eina_strbuf_append(markup_buf, node->string);
	  eina_strbuf_append(markup_buf, " underline2_color=#");
	  eina_strbuf_append(markup_buf, node->string);
	  eina_strbuf_append_char(markup_buf, '>');
	}
	else
	{
	  eina_strbuf_append(markup_buf, "<underline=double underline_color=#");
	  sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	  eina_strbuf_append(markup_buf, color);
	  eina_strbuf_append(markup_buf, " underline2_color=#");
	  eina_strbuf_append(markup_buf, color);
	  eina_strbuf_append_char(markup_buf, '>');
	}

	underlineset++;	
      }

      /* font size */
      else if (strcmp(buf, "fs") == 0)
      {
	char store[10];	

	if (fontset == 0)
	{
	  eina_strbuf_append(markup_buf, "<font_size=");
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "EOF encountered while getting font size", return 0);
	  /* integer divison yields the floor value, which is used as the font size as fractional font size is currently not supported */
	  // todo: implement support for fractional font size in text block.
	  c /= 2;
	  sprintf(store, "%d", c);
	  eina_strbuf_append(markup_buf, store);
	  eina_strbuf_append_char(markup_buf, '>');
	  fontset = 1;
	}
	else
	{
	  fscanf(fp, "%d", &c);
	  CHECK_EOF(fp, "EOF encountered while skipping font size", return 0);
	}
      }

      /* handle foreground colour */
      else if (strcmp(buf, "cf") == 0)
      {
	Ertf_Color *node;
	if (foregroundset)
	{
	  eina_strbuf_append(markup_buf, "</>");
	}
	eina_strbuf_append(markup_buf, "<color=#");
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "EOF encountered while reading colour number", return 0);
	node = (Ertf_Color *) eina_array_data_get(color_table, c);
	if(!node){
	  ERR("invalid node.");
	  exit(1);
	}
	eina_strbuf_append(markup_buf, node->string);
	eina_strbuf_append_char(markup_buf, '>');
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
	  eina_strbuf_append(markup_buf, "</>");
	}
	eina_strbuf_append(markup_buf, "<backing=on backing_color=#");
	fscanf(fp, "%d", &c);
	CHECK_EOF(fp, "EOF encountered while reading background colour number", return 0);
	node = (Ertf_Color *)eina_array_data_get(color_table, c);
	eina_strbuf_append(markup_buf, node->string);
	eina_strbuf_append_char(markup_buf, '>');
	backgroundset++;
      }

      /* handle strikethrough */
      else if (strcmp(buf, "strike") == 0)
      {
	if (!strikethroughset)
	{
	  char color[9];
	  eina_strbuf_append(markup_buf, "<strikethrough=on strikethrough_color=#");
	  sprintf(color, "%02x%02x%02xff", current_r, current_g, current_b);
	  eina_strbuf_append(markup_buf, color);
	  eina_strbuf_append_char(markup_buf, '>');
	  strikethroughset++;
	}
      }

      /* handle target */
      else if (strcmp(buf, "*") == 0)
      {
	/* They not supported in the prototype. */
	while ((c = fgetc(fp)) != EOF && c != '}')
	  ;
	CHECK_EOF(fp, "EOF reached while handling unsupported target", return 0);
	ungetc(c, fp);
      }

      /* unsupported or unrecognised control tag */
      else
      {
	INFO("skipped control tag `%s' and at line#%d", buf, _line);
      }

      /* read till next delimiter */
      while ((c = fgetc(fp)) != '\\' &&
	      c != ' '               &&
	      c != '{'               &&
	      c != '}'
	     )
	CHECK_EOF(fp, "EOF reached while skipping control info", return 0);
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
      if (c == '\n' || c == '\r')
      {
	/* Since unnecessary newlines are added by rtf writers, they are ignored. The tag <br> is used when \line is encountered. */

	// for debugging
	_line++;
      }
      else if (c == '&')
      {
	eina_strbuf_append(markup_buf, "&amp;");
      }
      else if (c == '<')
	eina_strbuf_append(markup_buf, "&lt;");
      else if (c == '>')
	eina_strbuf_append(markup_buf, "&gt;");
      else
      {
	eina_strbuf_append_char(markup_buf, c);
      }
    }
  }

  DBG("EOF encountered while looping for control word");
  return 0;

 success:
  if (boldset)
  {
    eina_strbuf_append(markup_buf, "</bold>");
  }
  if (italicset)
  {
    eina_strbuf_append(markup_buf, "</em>");
  }
  if (fontset)
  {
    eina_strbuf_append(markup_buf, "</>");
  }
  if (foregroundset)
  {
    eina_strbuf_append(markup_buf, "</>");
  }
  if (backgroundset)
  {
    eina_strbuf_append(markup_buf, "</>");
  }
  if (strikethroughset)
  {
    eina_strbuf_append(markup_buf, "</>");
  }
  if (underlineset)
  {
    eina_strbuf_append(markup_buf, "</>");
  }

  return 1;
}
