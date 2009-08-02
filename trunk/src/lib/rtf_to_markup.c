#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "input.h"

int ertf_paragraph_translate(FILE *);
static int ertf_group_translate(FILE *, int);

  // \plain is ignored and the text part is copied as markup and evas
  // textblock uses the style set by default

  // todo: \deleted not implemented in the initial version

  // \dn<N> translates to ?

  // \f translates to ? Can the font be set in the markup or does it have to be
  // set in textblock style? In the latter case, when a new group is encountered
  // in the rtf document a new style can be created in the style eina_array and
  // its name can be used in generating the markup. In the former case, it would
  // easier if something like <font=Verdana ...> is allowed within markup

  // \strike translates to <strikethough=on ...>

  // \scaps translates to ?

  // \uld translates to ?


int ertf_paragraph_translate(FILE *fp){
  int c;
  char buf[12];

  printf("<p>");
  strcpy(markup+ertf_markup_position, "<p>");
  ertf_markup_position+=3;

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '\n':// ignore it
      continue;

      /* get control word */
    case '\\':
      fscanf(fp, "%[^ 0123456789\\{]", buf);
      CHECK_EOF(fp, "ertf_paragraph_translate: end-of-file encountered while "
		"retrieving control word.\n", return 0);

      /* reset to default character formatting */
      if(strcmp(buf, "plain")==0){
      }

      /* get the style number */
      else if(strcmp(buf, "s")==0){
	// todo: ensure that all stylesheet entries have been parsed
	// todo: add the relevant style string to markup
      }

      /* end of paragraph */
      else if(strcmp(buf, "par")==0){
	// todo: ensure that </p> is defined in style string
	printf("</p>");

	strcpy(markup+ertf_markup_position, "</p>");
	ertf_markup_position+=4;
	return 1;
      }

      /* unsupported/unrecognised control tag */
      else{
	fprintf(stderr, "Skipped control tag `%s'\n", buf);

	/* read till next delimiter */
	while((c=fgetc(fp))!='\\' && c != ' ' && c!='{')
	  CHECK_EOF(fp, "ertf_paragraph_translate: EOF reached while skipping"
		    " control info.\n", return 0);
	ungetc(c, fp);
      }

      break;

      /* handle group */
    case '{':
      if(!ertf_group_translate(fp, 0)){
	return 0;
      }
      break;

    default:
      fprintf(stderr, "ertf_paragraph_translate: unrecognised control"
	      " character `%c'\n", c);
    }
  }
  fprintf(stderr, "ertf_paragraph_translate: EOF encountered while looping for"
	  " control word.\n");
  return 0;
}

// align = 0 for left and 1 for right
static int ertf_group_translate(FILE *fp, int align){
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
	// todo: either insert align=right in markup or in a style
	printf("<right>");

	strcpy(markup+ertf_markup_position, "<right>");
	ertf_markup_position+=7;

	if(!ertf_group_translate(fp, 1))
	  return 0;
	printf("</right>");

	strcpy(markup+ertf_markup_position, "</right>");
	ertf_markup_position+=8;
      }

      /* left aligned text */
      else if(strcmp(buf, "ltrch")==0 && align){
	return 1;	
      }

      /* italicized text */
      else if(strcmp(buf, "i")==0){
	if(!isdigit(c=fgetc(fp)))
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_group_translate: EOF encountered while checking for italicisation.\n", return 0);
	// todo: find relevant markup
      }

      /* bold text */
      else if(strcmp(buf, "b")==0){
	if(!isdigit(c=fgetc(fp)))
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_group_translate: EOF encountered while checking for bold text.\n", return 0);
	// todo: find relevant markup
      }

      /* font entry number */
      else if(strcmp(buf, "f")==0){
	if(!isdigit(c=fgetc(fp)))
	  ungetc(c, fp);
	CHECK_EOF(fp, "ertf_group_translate: EOF encountered while checking for bold text.\n", return 0);
	// todo: find relevant markup
      }

      /* underline */
      else if(strcmp(buf, "ul")==0){
	// todo: check if underline colour needs to be specified
	printf("<underline=on>");

	strcpy(markup+ertf_markup_position, "<underline=on>");
	ertf_markup_position+=14;
	if(!ertf_group_translate(fp, align))
	  return 0;
	printf("</>");
	strcpy(markup+ertf_markup_position, "</>");
	ertf_markup_position+=3;
      }
      
      /* font size */
      else if(strcmp(buf, "fs")==0){
	/* use the digit character instead of reading an int and converting it back to character for markup */

	printf("<font_size=");

	strcpy(markup+ertf_markup_position, "<font_size=");
	ertf_markup_position+=11;
	while(isdigit(c=fgetc(fp))){
	  CHECK_EOF(fp, "ertf_group_translate: EOF encountered while getting font size.\n", return 0);
	  printf("%c", c);

	  markup[ertf_markup_position]=c;
	  ertf_markup_position++;
	}
	ungetc(c, fp);
	printf(">");

	markup[ertf_markup_position]='>';
	ertf_markup_position++;
	if(!ertf_group_translate(fp, align))
	  return 0;
	printf("</>");

	strcpy(markup+ertf_markup_position, "</>");
	ertf_markup_position+=3;
      }

      /* unrecognised/unsupported control word */
      else{
	fprintf(stderr, "ertf_group_translate: Skipped control tag `%s'\n", buf);

	/* read till next delimiter */
	while((c=fgetc(fp))!='\\' && c != ' ' && c!='{')
	  CHECK_EOF(fp, "ertf_group_translate: EOF reached while skipping control info.\n", return 0);
	ungetc(c, fp);
      }

      break;

      /* handle nested group */
    case '{':
      if(!ertf_group_translate(fp, align))
	return 0;
      break;

      /* end of group */
    case '}':
      return 1;

      /* handle target */
    case '*':
      /* They not supported in the prototype. */
      while((c=fgetc(fp))!=EOF && c!='}')
	;
      CHECK_EOF(fp, "ertf_group_translate: EOF reached while handling unsupported target.\n", return 0);
      /* The end brace of the group is handled here and the loop continues with the next control word. */
      break;

    default:
      printf("%c", c);
      markup[ertf_markup_position]=c;
      ertf_markup_position++;
    }
  }// end of while

  fprintf(stderr, "ertf_group_translate: End-of-file reached.%d\n", c);
  // todo: confirm if this should return one and complete the string
  // todo: may be keep partial rendering optional
  return 0;
  
}
