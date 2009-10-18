#include <string.h>

//#define NDEBUG
#include <assert.h>

#include "Ertf.h"
#include "GUI.h"
#include "ertf_private.h"

FILE *fstream;

// Whenever `{' is encountered, the counter is incremented; and it is
// decremented when a `}' is encountered. If by the end of the parsing phase,
// the counter is non-zero, the rtf file is erroneous and a message to displayed
// to the user.

int bracecount = 0;
int version;
char charset[6];

void readloop();
void init_parser();
int shutdown_parser();

int
main(int argc, char **argv)
{
  assert(argc > 1);

  if ((fstream = fopen(argv[1], "r")) == NULL)
  {
    fprintf(stderr, "Cannot open %s\n", argv[1]);
  }
  else
  {
    if (!ertf_init())
      return -1;
    // todo: use setvbuf() to properly buffer the stream before reading
    init_parser();    
    // when the markup is generated, the parser can be shut down
    shutdown_parser();
    init_gui();
    shutdown_gui();
    ertf_shutdown();
  }
  return 0;
}


void
init_parser()
{
  char str[5];
  int c;// to allow for EOF

  if ((c = getc(fstream)) == EOF)
  {
    // todo:display blank textblock for empty file
  }
  else if (c != '{')
  {
    // An rtf file should start with `{\rtf'
    fprintf(stderr, "invalid rtf file\n");
  }
  else if (fscanf(fstream, "%4s", str), strcmp(str, "\\rtf") != 0 )
  {
    fprintf(stderr, "rtf version unspecified.\n");
  }
  else if ((fscanf(fstream, "%d\\%c", &version, &str[0]), str[0] != 'a') &&
	   str[0] != 'p' &&
	   str[0] != 'm'
	   )
  {
    // todo:improve the if  condition for full word checking
    fprintf(stderr, "charset not defined\n");
  }
  else
  {
    //increase brace count
    bracecount++;

    // store charset
    ungetc(str[0], fstream);
    fscanf(fstream, "%[^\\{]", charset);

    // start reading the document char by char
    readloop();
  }  
}


void
readloop()
{
  char control_word[30];
  int c;
  
  ertf_markup_position = 0;
  markup = NULL;

  while ((c = getc(fstream)) != EOF)
  {    
    switch (c)
    {
    case '{':
      bracecount++;
      break;
    case '}':
      bracecount--;
      break;
    case '\\'://todo: perform the control operation
      fscanf(fstream, "%[^ {\\;0123456789]", control_word);
      // Interestingly, a semi-colon delimits the "\colortbl" keyword sometimes

      CHECK_EOF(fstream, "readloop: EOF encountered.\n", return);

      /* font table */
      if (strcmp(control_word, "fonttbl") == 0)
      {
	if (ertf_font_table(fstream))
        {	
	  printf("Successfully created font table.\n");
	  bracecount--;
	}
        else
	  printf("failure in creating font table.\n");

	/* color table */
      }
      else if (strcmp(control_word, "colortbl") == 0)
      {
	if (ertf_color_table(fstream))
        {
	  printf("Successfully created color table.\n");
	  bracecount--;
	}
        else
	  printf("failure in creating color table.\n");

	/* stylesheet */
      }
      else if (strcmp(control_word, "stylesheet") == 0)
      {
	if (ertf_stylesheet_parse(fstream))
        {
	  printf("Successfully created stylesheet table.\n");
	  bracecount--;
	}
        else
	  printf("failure in creating stylesheet table.\n");
      }

      /* paragraph */
      else if (strcmp(control_word, "pard") == 0)
      {
	ertf_markup_add("<p>", 3);
	if (ertf_paragraph_translate(fstream, 0))
        {
	  printf("Successfully parsed a paragraph.\n");
	}
        else
	  printf("failure parsing parapgraph.\n");
      }

      /* default font */
      else if (strcmp(control_word, "deff") == 0)
      {
	fscanf(fstream, "%d", &_ertf_default_font);      
      }

      /* handle paper height */
      else if (strcmp(control_word, "paperh") == 0)
      {
	fscanf(fstream, "%d", &_ertf_paper_height);
      }

      /* handle paper width */
      else if (strcmp(control_word, "paperw") == 0)
      {
	fscanf(fstream, "%d", &_ertf_paper_width);
      }

      /* handle left margin */
      else if (strcmp(control_word, "margl") == 0)
      {
	fscanf(fstream, "%d", &_ertf_margin_left);
      }

      /* handle right margin */
      else if (strcmp(control_word, "margr") == 0)
      {
	fscanf(fstream, "%d", &_ertf_margin_right);
      }

      /* handle top margin */
      else if (strcmp(control_word, "margt") == 0)
      {
	fscanf(fstream, "%d", &_ertf_margin_top);
      }

      /* handle bottom margin */
      else if (strcmp(control_word, "margb") == 0)
      {
	fscanf(fstream, "%d", &_ertf_margin_bottom);
      }

      /* default tab size in twips */
      else if (strcmp(control_word, "deftab") == 0)
      {
	fscanf(fstream, "%d", &c);
	// todo: look for possible implementation in textblock, otherwise the tag is useless.
      }

      else if (strcmp(control_word, "info") == 0)
      {
	if (ertf_summary(fstream))
	  printf("Successfully parsed information section.\n");
	else
	  printf("failure parsing information about the file.\n");
      }

      /* unrecognised control word */
      else
      {
	fprintf(stderr, "readloop: skipping unrecognised control word %s\n", control_word);
      }

      break;

    default:
      fprintf(stderr, "readloop: skipped control char `%c'\n", c);
    }
  }

  markup[ertf_markup_position] = '\0';
  printf("%d\nmarkup:\n%s\n", ertf_markup_position, markup);
  // When end-of-file is reached, check if  parsing is complete. In case,
  // it is not, print an error message stating "incomplete rtf file".
  if (bracecount)
    fprintf(stderr, "readloop: Ill-formed rtf - inconsistent use of braces.\n");
}

int
shutdown_parser()
{
  fclose(fstream);
  // free tables used by the parser
  if (color_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Color         *color;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(color_table, i, color, iterator)
      free(color);
    eina_array_free(color_table);
  }
  if (font_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Font_Node     *font;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(font_table, i, font, iterator)
      free(font);
    eina_array_free(font_table);
  }
  if (stylesheet_table)
  {
    Eina_Array_Iterator iterator;
    Ertf_Stylesheet    *stylesheet;
    unsigned int        i;

    EINA_ARRAY_ITER_NEXT(stylesheet_table, i, stylesheet, iterator)
      free(stylesheet);
    eina_array_free(stylesheet_table);
  }

  return 1;
}
