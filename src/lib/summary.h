#ifndef ERTF_SUMMARY_H_
#define ERTF_SUMMARY_H_


typedef struct summary{
  // todo: check if there should be a limitation to the length of the strings
  char *author;
  char *title;
  char *subject;
  char *operator;
  char *keywords;
  char *comment;
  int version;
  char *doccomm;
  int internal_version;
  int pages;
  long int words;
  long int chars;
  int internal_ID;
  // todo: add time variables
} Ertf_Info;

Ertf_Info *doc_info;

int ertf_summary(FILE *);


#endif /* ERTF_SUMMARY_H_ */
