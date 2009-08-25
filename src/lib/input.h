#ifndef ERTF_INPUT_H_
#define ERTF_INPUT_H_


#include <stdio.h>


/*
 * This enables cleaner EOF checking.
 */
#define CHECK_EOF(STREAM, MESSAGE, RECOVERY) if (feof(STREAM)){fprintf(stderr, MESSAGE);RECOVERY;}

/* The indicator to current location shall be maintained by the accessing*/
/* programs. */
int ertf_markup_position;
char *markup;

int ertf_markup_add(char *, int);

#endif /* ERTF_INPUT_H_ */
