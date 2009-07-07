#include "colour.h"
#include <stdlib.h>
#include <string.h>

static int ertf_colour_add(FILE *);
/*
 * This function creates a colour table for an rtf file. It returns 1 upon 
 * success and 0 in case of failure.
 */
int ertf_colour_table(FILE *fp){
  int c;

  // todo: remove debug msg
  printf("Inside colour table handler.\n");

  // initialize eina array module
  if(!eina_array_init()){
    fprintf(stderr, "Error during initialization of eina error module.\n");
    return 0;
  }

  // create an eina array
  colour_table=eina_array_new(7);
  if(!colour_table){
    eina_array_shutdown();
    // In case of success, the eina array module shall be shut down by ertf
    // clean up functions when the app is closed.
    return 0;
  }

  // ready to parse the colour table now
  while((c=fgetc(fp))!=EOF){
    switch(c){
      COLOUR *node;
    case ';':// indicates default colour
      node=(COLOUR *)malloc(sizeof(COLOUR));
      // todo: assign default RGB values to this node
      eina_array_push(colour_table, node);
      break;

    case '\\':
      ungetc(c, fp);
      if(ertf_colour_add(fp))
	continue;
      else
	fprintf(stderr, "colortbl: Ill-formed rtf\n");
      break;

    case '}':// end of colour table
      return 1;

    default:
      fprintf(stderr, "colortbl: Ill-formed rtf");
      return 0;
    }
  }
  fprintf(stderr, "End of file reached in colour table.\n");
  return 0;
}
/*
 * This function adds an entry to the colour table. It returns 1 on success and
 * 0 in case of failure.
 */
#define RED 1
#define GREEN 2
#define BLUE 4
static int ertf_colour_add(FILE *fp){
  char colour[7];
  int index;
  int set=0;
  COLOUR *node=(COLOUR *)malloc(sizeof(COLOUR));
  int c;

  // todo: remove debug msg
  printf("Inside colour entry parser.\n");

  while((c=fgetc(fp))!=EOF){
    switch(c){
    case '\\':
      ungetc(c, fp);
      fscanf(fp, "%[^0123456789]", colour);
      if(feof(fp)) goto err;
      fscanf(fp, "%d", &index);// todo: do error checking for range of rgb value
      if(feof(fp)) goto err;
      if(strcmp(colour, "\\green") == 0){
	if(!(set & GREEN))
	  node->g = index;
	else
	  fprintf(stderr, "ertf_colour_add: multiple values for same colour.\n");
	// todo: confirm if this should only be logged and not stopped at
      }else if(strcmp(colour, "\\red")==0)
	node->r=index;
      // todo: add set/unset check as above
      else if(strcmp(colour, "\\blue")==0)
	node->b=index;
      // todo: add set/unset check as above
      else
	// continue as the tag is not recognised and should be therefore skipped
	fprintf(stderr, "Warning: skipped tag ``%s\".\n", colour);
      break;
    case ';':// todo: error checking on success of the push operation
      eina_array_push(colour_table, node);
      return 1;
    default:
      fprintf(stderr, "ertf_colour_add: Ill-formed rtf file.\n");
      goto err;
    }    
  }
 err:
  free(node);
  return 0;
}
