#include "stylesheet.h"
#include "colour.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static int ertf_stylesheet_add(FILE *);
int ertf_stylesheet_parse(FILE *fp){
  int c;

  // todo: remove debug msg
  printf("Inside stylesheet parser.\n");

  // initialize eina array module
  if(!eina_array_init()){
    fprintf(stderr, "Error during initialization of eina error module.\n");
    return 0;
  }

  // create an eina array
  stylesheet_table=eina_array_new(7);
  if(!stylesheet_table){
    eina_array_shutdown();
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    return 0;
  }

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '{':
      if(!ertf_stylesheet_add(fp)){
	fprintf(stderr, "ertf_stylesheet_parse: Ill-formed rtf.\n");
	return 0;
      }
      if((c=fgetc(fp))==EOF || c!='}'){
	// end braces for each style is consumed here.
	goto err_loop;
      }      
      break;

    case '}':// end of stylesheets
      return 1;// successful return

    default:
      fprintf(stderr, "ertf_stylesheet_parse: invalid character.\n");
      return 0;
    }
  }

 err_loop:
  fprintf(stderr, "End of file reached in stylesheet table.\n");
  return 0;// unsuccessful return
}


static int ertf_stylesheet_add(FILE *fp){
  int c;
  char buf[12];
  STYLE *style=(STYLE *)malloc(sizeof(STYLE));
  unsigned int colour_max = eina_array_count_get(colour_table);

  // todo: remove debug msg
  printf("Inside stylesheet entry parser.\n");

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '\\':// get the control word
      fscanf(fp, "%[^ 0123456789\\]", buf);
      // read until a delimiter is encountered

      if(feof(fp)){
	fprintf(stderr, "ertf_stylesheet_add: Ill-formed rtf.\n");
	goto error;
      }

      // todo: use pattern matching technique / hashing and benchmark them
      // to decide the optimal alternative to the if-else ladder
      switch(buf[0]){
      case 'c':
	if(strcmp(buf+1,"f")==0){
	  // read the colour table entry number
	  fscanf(fp, "%u", &style->foreground_colour);
	  if(style->foreground_colour >= colour_max){
	    fprintf(fp, "ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	}else if(strcmp(buf+1, "b")==0){
	  fscanf(fp, "%u", &style->background_colour);
	  if(style->background_colour >= colour_max){
	    fprintf(fp, "ertf_stylesheet_add: stylesheet colour not in colour table\n");
	    goto error;
	  }
	}
	break;

      case 's':
	if(buf[1] == '\0'){
	  // read font number
	  fscanf(fp, "%d", &style->font_number);	 
	}else if (strcmp(buf+1,"basedon")==0){
	}
	break;

      default:// skip unrecognised/unsupported control word
	// todo: check if NOR conversion simplifies it
	while((c=fgetc(fp))!= EOF  && c != '\\' && !isdigit(c))
	  ;
	if(c == EOF){
	  fprintf(stderr, "ertf_stylesheet_add: end of file encountered while skipping unrecognised tag\n");
	  goto error;
	}else if (c=='\\'){
	  ungetc(c, fp);
	}
      }
      break;

    case '{':
      break;

    case ';':// end of style
      eina_array_push(stylesheet_table, style);
      return 1;

    case ' ':// get style name
      fscanf(fp, "%[^;]", style->name);
      if(feof(fp)){
	fprintf(stderr, "ertf_stylesheet_add: end of file encountered while reading stylesheet name.\n");
	goto error;
      }
      break;

    default:
      fprintf(stderr, "ertf_stylesheet_add: unrecognised control character '%c'.\n", c);
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
