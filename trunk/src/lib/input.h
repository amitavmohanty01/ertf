#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
/*
 * This enables cleaner EOF checking.
 */
#define CHECK_EOF(STREAM, MESSAGE, RECOVERY); if(feof(STREAM)){fprintf(stderr, MESSAGE);RECOVERY;}

/* The indicator to current location shall be maintained by the accessing*/
/* programs. */
int ertf_markup_position;
char markup[2048];
#endif
