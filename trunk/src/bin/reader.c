#include "input.h"
#include "font.h"
#include "colour.h"
#include "summary.h"
#include "stylesheet.h"
#include "GUI.h"
#include "rtf_to_markup.h"
#include <string.h>
//#define NDEBUG
#include <assert.h>

FILE *fstream;

// Whenever `{' is encountered, the counter is incremented; and it is
// decremented when a `}' is encountered. If by the end of the parsing phase,
// the counter is non-zero, the rtf file is erroneous and a message to displayed
// to the user.
int bracecount=0;
int default_font_node; // for \deff<N> control word
int version;
char charset[6];

void readloop();
void init_parser();
int shutdown_parser();

int main(int argc, char **argv){
  assert(argc > 1);
  if((fstream = fopen(argv[1], "r"))== NULL){
    fprintf(stderr, "Cannot open %s\n", argv[1]);
  }else{
  // todo: use setvbuf() to properly buffer the stream before reading
    init_parser();
    // when the markup is generated, the parser can be shut down
    shutdown_parser();
    init_gui();
    shutdown_gui();
  }
  return 0;
}


void init_parser(){
  int c;// to allow for EOF
  char str[5];

  if((c=getc(fstream)) == EOF){
    // todo:display blank textblock for empty file
  }else if(c!='{'){
    // An rtf file should start with `{\rtf'
    fprintf(stderr, "invalid rtf file\n");
  }else if(fscanf(fstream, "%4s", str), strcmp(str, "\\rtf")!=0 ){
    fprintf(stderr, "rtf version unspecified.\n");
  }else if((fscanf(fstream, "%d\\%c", &version, &str[0]), str[0] != 'a') &&
	   str[0] != 'p' &&
	   str[0] != 'm'
	   ){
    // todo:improve the if condition for full word checking
    fprintf(stderr, "charset not defined\n");
  }else{
    //increase brace count
    bracecount++;

    // store charset
    ungetc(str[0], fstream);
    fscanf(fstream, "%[^\\{]", charset);

    // start reading the document char by char
    readloop();
  }  
}


void readloop(){
  int c;
  char control_word[30];
  while((c=getc(fstream))!=EOF){
    //todo:connect c to the required stream to read characters
    switch(c){
    case '{':
      bracecount++;
      break;
    case '}':
      bracecount--;
      break;
    case '\\'://todo:perform the control operation
      fscanf(fstream, "%[^ {\\;0123456789]", control_word);
      // Interestingly, a semi-colon delimits the "\colortbl" keyword sometimes

      CHECK_EOF(fstream, "readloop: EOF encountered.\n", return);

      /* font table */
      if(strcmp(control_word, "fonttbl")==0){
	if(ertf_font_table(fstream)){
	  // todo: check for success from return value
	  printf("Successfully created font table.\n");
	  bracecount--;
	}else
	  printf("failure in creating font table.\n");

	/* colour table */
      }else if(strcmp(control_word, "colortbl")==0){
	if(ertf_colour_table(fstream)){
	  printf("Successfully created colour table.\n");
	  bracecount--;
	}else
	  printf("failure in creating colour table.\n");

	/* stylesheet */
      }else if(strcmp(control_word, "stylesheet")==0){
	if(ertf_stylesheet_parse(fstream)){
	  printf("Successfully created stylesheet table.\n");
	  bracecount--;
	}else
	  printf("failure in creating stylesheet table.\n");
      }

	/* paragraph */
      else if(strcmp(control_word, "pard")==0){
	if(ertf_paragraph_translate(fstream)){
	  printf("Successfully parsed a paragraph.\n");
	  bracecount--;
	}else
	  printf("failure parsing parapgraph.\n");
      }

      break;

    default://write c
      ;
    }
  }
  // When end-of-file is reached, check if parsing is complete. In case,
  // it is not, print an error message stating "incomplete rtf file".
  if(bracecount)
    fprintf(stderr, "readloop: Ill-formed rtf - inconsistent use of braces.\n");
}


int shutdown_parser(){
  // todo: close the file stream
  fclose(fstream);
  // free colour tables used by the parser
  if(colour_table)
    eina_array_free(colour_table);
  if(font_table)
    eina_array_free(font_table);
  if(stylesheet_table)
    eina_array_free(stylesheet_table);

  // shutdown eina array module
  if(!eina_array_shutdown())
    return 0;
  return 1;
}
