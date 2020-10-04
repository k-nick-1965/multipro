/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 *\
			     memadd.h

		       Additional Mem  library

			(C)  Kaling N.V. 1993

\* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#if     !defined(__MEMADD_H)
#define __MEMADD_H

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#include <mem.h>

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
extern "C" {
#endif

/* 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

void         *memsetw(void *s, int c, size_t n) ;

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
}
#endif

#endif  /* __MEMADD_H */

