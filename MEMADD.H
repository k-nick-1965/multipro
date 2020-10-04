/* �������������������������������������������������������������������� *\
			     memadd.h

		       Additional Mem  library

			(C)  Kaling N.V. 1993

\* �������������������������������������������������������������������� */

#if     !defined(__MEMADD_H)
#define __MEMADD_H

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#include <mem.h>

/* �������������������������������������������������������������������� */

#ifdef __cplusplus
extern "C" {
#endif

/* �������������������������������������������������������������������� */

void         *memsetw(void *s, int c, size_t n) ;

/* �������������������������������������������������������������������� */

#ifdef __cplusplus
}
#endif

#endif  /* __MEMADD_H */
