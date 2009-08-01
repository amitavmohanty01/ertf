#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
/*
 * This enables cleaner EOF checking.
 */
#define CHECK_EOF(STREAM, MESSAGE, RECOVERY); if(feof(STREAM)){fprintf(stderr, MESSAGE);RECOVERY;}
char markup[1024];
#endif
