#ifndef SUMMARY_H
#define SUMMARY_H
int ertf_summary(FILE *);
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
}INFO;
INFO *doc_info;
#endif
