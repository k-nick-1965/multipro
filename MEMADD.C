/* ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ *\
			     memadd.c

		       Additional Mem  library

			(C)  Kaling N.V. 1993

\* ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */

#include <mem.h>
#include <dos.h>

/* ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ */

void *memsetw(void *s, int c, size_t n) {
  asm{
    push  es; push di; push ax; push cx;
  }
  _ES = FP_SEG(s);
  asm push es;
  _DI = FP_OFF(s);
  asm{    /* ax -> es:di */
    pop   es
    mov   cx,n
    sar   cx,1
    mov   ax,c
rep stosw
    pop   cx; pop ax; pop di; pop es;
  }
  return s;
}

/* ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ */
