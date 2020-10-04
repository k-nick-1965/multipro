/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 *\
			     stradd.h

		      Additional String library

			(C)  Kaling N.V. 1993

\* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#if     !defined(__STRADD_H)
#define __STRADD_H

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#include <string.h>

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
extern "C" {
#endif

/* 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

void    _Cdecl  strincl(char *point, char *in) ;

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
}
#endif

#endif  /* __STRADD_H */

