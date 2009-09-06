#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include <Evas.h>

#include "ertf_color.h"
#include "ertf_font.h"
#include "ertf_stylesheet.h"
#include "ertf_private.h"


typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
  char charset[6];
  Evas_Object *obj;
  Evas_Object *textblock;
  char *filename;
  FILE *f;
  int bracecount;
  int version;
};

/* local subsystem functions */
static void _smart_reconfigure(Smart_Data *sd);
static void _smart_init(void);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object *obj);

/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

Evas_Object *
esmart_rtf_add (Evas *evas)
{
  _smart_init ();

  return evas_object_smart_add (evas, _e_smart);
}

Eina_Bool
esmart_rtf_file_set(Evas_Object *obj, const char *filename)
{
  char control_word[30];
  char        str[5];
  Smart_Data *sd;
  int         c;

  if (!filename || (*filename == '\0'))
    return EINA_FALSE;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return EINA_FALSE;

  if ((filename) &&
      (sd->filename) &&
      (!strcmp (filename, sd->filename))) return EINA_TRUE;

  if (sd->f)
    fclose(sd->f);
  sd->f = NULL;

  if (sd->filename)
      free(sd->filename);

  sd->filename = strdup(filename);
  if (!sd->filename)
    return EINA_FALSE;

  sd->f = fopen(filename, "rb");
  if (!sd->f)
    goto free_filename;

  /* init parser */

  if ((c = getc(sd->f)) == EOF)
  {
    // todo:display blank textblock for empty file
  }
  else if (c != '{')
  {
    // An rtf file should start with `{\rtf'
    goto free_f;
  }
  else if (fscanf(sd->f, "%4s", str), strcmp(str, "\\rtf") != 0 )
  {
    fprintf(stderr, "rtf version unspecif ied.\n");
  }
  else if ((fscanf(sd->f, "%d\\%c", &sd->version, &str[0]), str[0] != 'a') &&
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
    sd->bracecount++;

    // store charset
    ungetc(str[0], sd->f);
    fscanf(sd->f, "%[^\\{]", sd->charset);
  }

  /* we parse the entire file */

  ertf_markup_position = 0;
  markup = NULL;

  while ((c = getc(sd->f)) != EOF)
  {
    switch (c)
    {
    case '{':
      sd->bracecount++;
      break;
    case '}':
      sd->bracecount--;
      break;
    case '\\'://todo:perform the control operation
      fscanf(sd->f, "%[^ {\\;0123456789]", control_word);
      // Interestingly, a semi-colon delimits the "\colortbl" keyword sometimes

      if (feof(sd->f))
      {
          fprintf(stderr, "readloop: EOF encountered.\n");
          goto free_f;
      }

      /* font table */
      if (strcmp(control_word, "fonttbl") == 0)
      {
	if (ertf_font_table(sd->f))
        {
	  printf("Successfully created font table.\n");
	  sd->bracecount--;
	}
        else
	  printf("failure in creating font table.\n");

	/* color table */
      }
      else if (strcmp(control_word, "colortbl") == 0)
      {
	if (ertf_color_table(sd->f))
        {
	  printf("Successfully created color table.\n");
	  sd->bracecount--;
	}
        else
	  printf("failure in creating color table.\n");

	/* stylesheet */
      }
      else if (strcmp(control_word, "stylesheet") == 0)
      {
	if (ertf_stylesheet_parse(sd->f))
        {
	  printf("Successfully created stylesheet table.\n");
	  sd->bracecount--;
	}
        else
	  printf("failure in creating stylesheet table.\n");
      }

      /* paragraph */
      else if (strcmp(control_word, "pard") == 0)
      {
	ertf_markup_add("<p>", 3);
	if (ertf_paragraph_translate(sd->f, 0))
        {
	  printf("Successfully parsed a paragraph.\n");
	}
        else
	  printf("failure parsing parapgraph.\n");
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
  if (sd->bracecount)
    fprintf(stderr, "readloop: Ill-formed rtf - inconsistent use of braces.\n");

  evas_object_textblock_text_markup_set(sd->textblock, markup);

  return EINA_TRUE;

 free_f:
  fprintf(stderr, "invalid rtf file\n");
  fclose(sd->f);
  sd->f = NULL;
 free_filename:
  free(sd->filename);
  sd->filename = NULL;

  return EINA_FALSE;
}

const char *
esmart_rtf_file_get(Evas_Object *obj)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return NULL;

  return sd->filename;
}


/* local subsystem globals */

static void
_smart_init(void)
{
  if (_e_smart) return;

  printf ("%s\n", __FUNCTION__);
  static const Evas_Smart_Class sc = {
    "ertf",
    EVAS_SMART_CLASS_VERSION,
    _smart_add,
    _smart_del,
    _smart_move,
    _smart_resize,
    _smart_show,
    _smart_hide,
    _smart_color_set,
    _smart_clip_set,
    _smart_clip_unset,
    NULL,
    NULL,
    NULL,
    NULL
  };
  _e_smart = evas_smart_class_new(&sc);
}

static void
_smart_add(Evas_Object *obj)
{
  Smart_Data           *sd;
  Evas                 *evas;
  Evas_Textblock_Style *st;

  sd = calloc(1, sizeof(Smart_Data));
  if (!sd) return;

  evas = evas_object_evas_get(obj);
  sd->obj = evas_object_rectangle_add(evas);
  evas_object_color_set(sd->obj, 255, 255, 255, 255);
  sd->textblock = evas_object_textblock_add(evas);
  sd->filename = NULL;
  sd->f = NULL;
  sd->bracecount = 0;
  sd->version = 0;
  evas_object_smart_member_add(sd->obj, obj);
  evas_object_smart_data_set(obj, sd);

  st = evas_textblock_style_new();
  evas_textblock_style_set(st,
                           "DEFAULT='font=Vera,Kochi font_size=8 align=left color=#000000 wrap=word'"
                           "center='+ font=Vera,Kochi font_size=10 align=center'"
                           "/center='- \n'"
                           "right='+ font=Vera,Kochi font_size=10 align=right'"
                           "/right='- \n'"
                           "blockquote='+ left_margin=+24 right_margin=+24 font=Vera,Kochi font_size=10 align=left'"
                           "h1='+ font_size=20'"
                           "red='+ color=#ff0000'"
                           "p='+ font=Vera,Kochi font_size=10 align=left'"
                           "/p='- \n'"
                           "br='\n'"
                           "tab='\t'");
  evas_object_textblock_style_set(sd->textblock, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(sd->textblock);
}

static void
_smart_del(Evas_Object *obj)
{
  Smart_Data         *sd;
  Eina_Array_Iterator iterator;
  unsigned int        i;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;

  if (color_table)
  {
    Ertf_Color         *color;

    EINA_ARRAY_ITER_NEXT(color_table, i, color, iterator)
      free(color);
    eina_array_free(color_table);
  }

  if (font_table)
  {
    Ertf_Font_Node     *font;

    EINA_ARRAY_ITER_NEXT(font_table, i, font, iterator)
      free(font);
    eina_array_free(font_table);
  }

  if (stylesheet_table)
  {
    Ertf_Stylesheet    *stylesheet;

    EINA_ARRAY_ITER_NEXT(stylesheet_table, i, stylesheet, iterator)
      free(stylesheet);
    eina_array_free(stylesheet_table);
  }

  if (sd->filename)
    free(sd->filename);
  if (sd->f)
    fclose(sd->f);
  evas_object_del(sd->obj);
  free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;

  evas_object_move (sd->obj, x, y);
  evas_object_move (sd->textblock, x, y);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;

  //   if ((sd->w == w) && (sd->h == h)) return;
  evas_object_resize (sd->obj, w, h);
  evas_object_resize (sd->textblock, w, h);
}

static void
_smart_show(Evas_Object *obj)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;
  evas_object_show(sd->obj);
  evas_object_show(sd->textblock);
}

static void
_smart_hide(Evas_Object *obj)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;
  evas_object_hide(sd->obj);
  evas_object_hide(sd->textblock);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;
  evas_object_color_set(sd->obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object * clip)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;
  evas_object_clip_set(sd->obj, clip);
  evas_object_clip_set(sd->textblock);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
  Smart_Data *sd;

  sd = evas_object_smart_data_get(obj);
  if (!sd) return;
  evas_object_clip_unset(sd->obj);
  evas_object_clip_unset(sd->textblock);
}
