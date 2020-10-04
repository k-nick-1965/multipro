/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 *\
			     stradd.h

		      Additional String library

			(C)  Kaling N.V. 1993

\* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#include <string.h>

/* 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

void strincl(char *point, char *in) {
int lin,l;
  l  =strlen(point);
  lin=strlen(in);
  memmove((point+lin),point,l+1);
  memmove(point,in,lin);
}

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */
