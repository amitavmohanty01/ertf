#ifndef ERTF_INPUT_H_
#define ERTF_INPUT_H_


#include <stdio.h>


/*
 * This enables cleaner EOF checking.
 */
#define CHECK_EOF(STREAM, MESSAGE, RECOVERY); if (feof(STREAM))\
{                                                              \
  ERR(MESSAGE);						       \
  RECOVERY;                                                    \
}

int ertf_tag_get(FILE *, char *);

int ertf_group_skip(FILE *);

#endif /* ERTF_INPUT_H_ */
