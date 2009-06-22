#define ANSI "ANSI"
#define MAC "Apple Macintosh"
#define PC "IBM(R) PC"
#define PCA "IBM PC Code Page 850, used by IBM Personal System/2(R)"
char *version; 
char *charset;
void ertf_init(){
  //1. verifies if file is in rtf format or not
  //2. determines the charset
  //3. maps/loads the file to memory
  //4. calls the parser to parse from rtf to markup
  //5. frees the memory map of the file
  //6. loads the GUI libs
}
void ertf_shutdown(){
  //1. unloads the GUI libs
  //2. frees the markup string
}
