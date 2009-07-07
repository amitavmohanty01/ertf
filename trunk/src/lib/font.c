#include "font.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
static int ertf_font_add(FILE *);
/*
 * This function creates a colour table for an rtf file. It returns 1 upon 
 * success and 0 in case of failure.
 */
int ertf_font_table(FILE *fp){
  int c;

  // todo: remove debug msg
  printf("Inside font table handler.\n");

  // initialize eina array module
  if(!eina_array_init()){
    fprintf(stderr, "Error during initialization of eina error module.\n");
    return 0;
  }

  // create an eina array
  font_table=eina_array_new(7);
  if(!font_table){
    eina_array_shutdown();
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    // todo: check for the same before final release
    return 0;
  }

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '{':// indicates more than one fonts likely
      if(!ertf_font_add(fp)) goto err_loop;
      if((c=fgetc(fp))==EOF && c!='}'){
	// end braces for each font enumeration is consumed here.
	goto err_loop;
      }      
      break;

    case '}':// end of font table group; time to return
      return 1;// successful return

    case '\\':
      ungetc(c, fp);
      if(!ertf_font_add(fp)) goto err_loop;
      break;

    default: goto err_loop;
    }
  }

 err_loop:
  fprintf(stderr, "Incorrect termination of file. Probably corrupted.\n");
  return 0;// unsuccessful return
}

// todo: check for multiple occurence of same tag in one entry
static int ertf_font_add(FILE *fp){
  char buf[10];
  int c;
  FONT *node=(FONT *)malloc(sizeof(FONT));
  // todo: rare situation these days, so the check may be removed.
  if(!node)fprintf(stderr, "Out of memory.\n");

  // todo: remove debug msg
  printf("Inside font entry parser.\n");

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '\\': //encountered a control word      
      fscanf(fp, "%[^ 0123456789]", buf);
      if(feof(fp))
	fprintf(stderr, "ertf_font_add: Ill-formed rtf.\n");
      if(strcmp(buf, "f")==0){
	fscanf(fp, "%d",&node->number);
      }else if(strcmp(buf, "fRoman")==0){
	strcpy(node->family, "Roman");
      } else if(strcmp(buf, "fswiss")==0){
	strcpy(node->family, "swiss");
      } else if(strcmp(buf, "fmodern")==0){
	strcpy(node->family, "modern");
      } else if(strcmp(buf, "fscript")==0){
	strcpy(node->family, "script");
      } else if(strcmp(buf, "fdecor")==0){
	strcpy(node->family, "decor");
      } else if(strcmp(buf, "ftech")==0){
	strcpy(node->family, "tech");
      } else if(strcmp(buf, "fnil")==0){
	strcpy(node->family, "default");
	// todo: after the multiple occurence check, the font family checks can
	// be modified to be done only once and rather have a bitwise check
	// run each time
      }else{// unrecognised or unsupported tag
	while((c=fgetc(fp))!= EOF  && c != '\\' && !isdigit(c))
	  ;
	if(c == EOF){
	  fprintf(stderr, "ertf_font_add: end of file encountered while skipping unrecognised tag\n");
	  goto error;
	}else if (c=='\\'){
	  ungetc(c, fp);
	}
      }
      break;

    case ' ':fscanf(fp, "%[^;]", node->name);
      if(feof(fp)){
	fprintf(stderr, "ertf_font_add: end of file encountered while reading font name.\n");
	goto error;
      }
      break;

    case';':// end of font entry
      eina_array_push(font_table, node);
      return 1;// successful return

    default:
      // todo: remove debug statement in final version
      fprintf(stderr, "ertf_font_add: unrecognised control character '%c'.\n", c);
      goto error;
    }
  }
  // end of file is reached
  // todo: remove debug statement in final version
  fprintf(stderr, "ertf_font_add: Ill-formed rtf.\n");
 error:
  free(node);
  return 0;
}
