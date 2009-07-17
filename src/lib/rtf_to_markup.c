#include <string.h>
#include <stdio.h>
#include "input.h"
int ertf_paragraph_translate(FILE *);
static int ertf_group_translate(FILE *);

  // \plain is ignored and the text part is copied as markup and evas
  // textblock uses the style set by default

  // \b transaltes to ?

  // \caps translates to touppercase() on the text part that follows

  // todo: \deleted not implemented in the initial version

  // \dn<N> translates to ?

  // \f translates to ? Can the font be set in the markup or does it have to be
  // set in textblock style? In the latter case, when a new group is encountered
  // in the rtf document a new style can be created in the style eina_array and
  // its name can be used in generating the markup. In the former case, it would
  // easier if something like <font=Verdana ...> is allowed within markup

  // \strike translates to <strikethough=on ...>

  // \ul translates to <underline=on ...>

  // \scaps translates to ?

  // \uld translates to ?


int ertf_paragraph_translate(FILE *fp){
  int c;
  char buf[12];

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '\n':// ignore it
      continue;

      /* get control word */
    case '\\':
      fscanf(fp, "%[^ 0123456789\\{]", buf);
      CHECK_EOF(fp, "ertf_paragraph_transalte: end-of-file encountered while "
		"retrieving control word.\n", return 0);

      // todo: process the control word

      /* reset to default character formatting */
      if(strcmp(buf, "plain")==0){
      }

      /* get the style number */
      else if(strcmp(buf, "s")==0){
      }

      /* end of paragraph */
      else if(strcmp(buf, "par")==0){
	return 1;
      }

      /* unsupported/unrecognised control tag */
      else{
      }

      break;

      /* handle group */
    case '{':
      ertf_group_translate(fp);
      break;

    default:
      fprintf(stderr, "ertf_paragraph_translate: unrecognised control character `%c'\n", c);
    }
  }
}


static int ertf_group_translate(FILE *fp){
  int c;
  char buf[12];

  while((c=fgetc(fp))!=EOF){
    switch(c){
      /* get control word */
    case '\\':
      fscanf(fp, "%[^ 0123456789\\{]", buf);
      CHECK_EOF(fp, "ertf_group_translate: EOF reached while reading control word.\n", return 0);

      /* right aligned text */
      if(strcmp(buf, "rtlch")==0){
	// todo: eithe insert align=right in markup or in a style
      }

      /* left aligned text */
      else if(strcmp(buf, "ltrch")==0){
	continue;
      }

      /* italicized text */
      else if(strcmp(buf, "i")==0){
	// todo: find relevant markup
      }

      /* bold text */
      else if(strcmp(buf, "b")==0){
      }

      /* font entry number */
      else if(strcmp(buf, "f")==0){
      }

      /* font size */
      else if(strcmp(buf, "fs")==0){
      }

      /* unrecognised/unsupported control word */
      else{
	fprintf(stderr, "Skipped control tag `%s'\n", buf);
      }

      break;

      /* handle nested group */
    case '{':
      break;

      /* end of group */
    case '}':
      return 1;

      /* handle target */
    case '*':
      /* They not supported in the prototype. */
      while((c=fgetc(fp))!=EOF && c!='}')
	;
      if(c==EOF){
	fprintf(stderr, "ertf_group_translate: EOF reached while handling unsupported target.\n");
	return 0;
      }
      /* The end brace of the group is handled here and the loop continues with the next control word. */
      break;

    default:
      fprintf(stderr, "ertf_group_translate: skipping control character `%c'\n", c);
    }
  }
}
