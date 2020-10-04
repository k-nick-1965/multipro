/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ *\
                              window.c

                           Window library

                         remembe TS Modula-2

                        (C)  Kaling N.V. 1993  28-Oct-93

\* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */

#include <alloc.h>;
#include <stdlib.h>;
#include <string.h>;
#include <mem.h>;
#include <dos.h>;
#include <stdio.h>;
#include <conio.h>;
#include "window.h"
#include "memadd.h"
#include "stradd.h"

#pragma  startup InitWindowLibrary 80

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern    void  NullProc();
extern    void* NullProcPoint();
extern    void _CopyMainBufToScreen(ScrBuf bf);
extern    void _MainWrStr(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char *S);

WindDef   FullScreenDef    = {0 , 0,79,24,FALSE,TRUE,LightGray,Black ,Black,Black,SingleFrame};
WindType  FullScreen       = NULL;
WindType  FirstWindow      = NULL;
WindType  CurrWT           = NULL;
ScrBuf    MainScreenBuffer = {0,0,0,NULL};
const     GuardConst       = 5252;
boolean   WriteOn          = TRUE;

struct UserDef { void            *ProcEnv;
                 WindType         WT;
                 struct UserDef  *Next;
                } ;
typedef struct UserDef  UserDefType;
typedef struct UserDef *UserList;

UserList  FirstUser     = NULL;
boolean   MultiP        = FALSE;
void      (*WLock)()    = NullProc;
void      (*WUnLock)()  = NullProc;
void*     (*CurrProc)() = NullProcPoint;
cardinal  B800          = 0xB800;

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void  NullProc(){;}
void* NullProcPoint() { return NULL; }

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void Beep(void) {
int i;
  WLock();
  sound(392);   delay(20);
  sound(440);   delay(30);
  sound(494);   delay(40);
  sound(523);   delay(50);
  nosound();
  WUnLock();
}


/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void FatalError(char *s) {
  WLock();
  _setcursortype(_NORMALCURSOR);
  _MainWrStr(MainScreenBuffer,10,10,Yellow,Black,s);
  _CopyMainBufToScreen(MainScreenBuffer);
  Beep();
  exit(1);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType CurrentWindow() {
UserList ul;
  if (!MultiP) {
    WCheckWindow(CurrWT);
    return CurrWT;
  }
  else {
    WLock();
    if (FirstUser==NULL) {
      // FatalError(" Fatal Error: Nill first window user.");
      WUnLock();
      return FullScreen;
    }
    ul=FirstUser;
    while (ul!=NULL) {
      if ( ul->ProcEnv == (void*) CurrProc()) {
        WCheckWindow(ul->WT);
        WUnLock();
        return ul->WT;
      }
      ul=ul->Next;
    }
    WCheckWindow(FirstWindow);
    WUnLock();
    return FirstWindow;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void SetCurrentWindow(WindType wt) {
UserList ul;
  WCheckWindow(wt);
  if (!MultiP) {
    CurrWT=wt;
  }
  else {
    WLock();
    ul=FirstUser;
    while (ul!=NULL) {
      if ( ul->ProcEnv == (void*) CurrProc()) {
        ul->WT=wt;
        WUnLock();
        return;
      }
      ul=ul->Next;
    }
    ul = (UserList) farmalloc(sizeof(UserDefType));
    if (ul==NULL) {FatalError(" Fatal Error: Too small memory for window user.");}
    ul->Next=FirstUser;
    ul->ProcEnv=(void*) CurrProc();
    ul->WT=wt;
    FirstUser=ul;
    WUnLock();
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void CloseWindowUser(WindType wt) {
UserList ul,*preul;
  WCheckWindow(wt);
  if (MultiP) {
    WLock();
    ul=FirstUser;
    preul=&FirstUser;
    while (ul!=NULL) {
      if ( ul->ProcEnv == (void*) CurrProc()) {
        *preul=ul->Next;
        farfree(ul);
        WUnLock();
        return;
      }
      preul=&ul->Next;
      ul=ul->Next;
    }
    WUnLock();
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _ClearWindow(WindType wt) {
  memsetw(wt->BufPointer,((wt->Back<<12)+(wt->Fore<<8)+32),wt->BufSize);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _PutOnTop(WindType wt) {
WindType p1,p2;
  WLock();
  if (FirstWindow==wt) FirstWindow=wt->Next;
  if (FirstWindow==NULL)
    FirstWindow=wt;
  else {
    p1=FirstWindow;
    while (p1!=NULL) {
      WCheckWindow(p1);
      if (p1==wt) p2->Next=wt->Next;
      else p2=p1;
      p1=p1->Next;
    }
    p2->Next=wt;
  }
  wt->Next=NULL;
  SetCurrentWindow(wt);
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _PutBeneath(WindType Twt, WindType Bwt) {
WindType Cur,Pre;
  WLock();
  if (FirstWindow==Twt) FirstWindow=Twt->Next;
  else {
    Cur=FirstWindow;
    while (Cur!=NULL) {
      WCheckWindow(Cur);
      if (Cur==Twt) {
        Pre->Next=Twt->Next;
        break;
      }
      Pre=Cur;
      Cur=Cur->Next;
    }
  }
  Cur=FirstWindow;
  while (Cur!=NULL) {
    WCheckWindow(Cur);
    if (Cur==Bwt) {
      Twt->Next=Cur->Next;
      Cur->Next=Twt;
      break;
    }
    Pre=Cur;
    Cur=Cur->Next;
  }
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType _Open(WindDef* wd) {
WindType wt;
Coord x,y;

  WLock();

  wt = (WindType) farmalloc(sizeof(MainWindDefType));
  WCheckPointer(wt);
  wt->Guard = GuardConst;
  wt->X1       = wd->X1;       wt->Y1     = wd->Y1;
  wt->X2       = wd->X2;       wt->Y2     = wd->Y2;
  wt->FrameOn  = wd->FrameOn;
  wt->Fore     = wd->Fore;     wt->Back   = wd->Back;
  wt->FrameF   = wd->FrameF;   wt->FrameB = wd->FrameB;
  wt->HideOff  = wd->HideOff;
  strcpy(wt->FrameDef,wd->FrameDef);

  wt->XC       = 0;            wt->YC = 0;
  wt->CursorOn = FALSE;
  wt->WrapOn   = FALSE;
  wt->BufSize  = (wt->X2-wt->X1+1) * (wt->Y2-wt->Y1+1) * 2;
  wt->BufPointer = (char*) farmalloc((wt->BufSize));
  WCheckPointer(wt->BufPointer);
  wt->Next       = NULL;
  wt->UpperTitleType = NoTitle;
  wt->LowerTitleType = NoTitle;
  wt->UpperTitleStr = NULL;
  wt->LowerTitleStr = NULL;

  _ClearWindow(wt);
  _PutOnTop(wt);

  WUnLock();

  return wt;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _Change(WindType nxt,Coord X1, Coord Y1, Coord X2, Coord Y2) {
WindType old;
cardinal my,wp,l,h,lo,ln,ho,hn;

  WLock();

  old = (WindType) farmalloc((sizeof(MainWindDefType)));
  WCheckPointer(old);
  *old=*nxt;
  nxt->X1 = X1;  nxt->Y1 = Y1;
  nxt->X2 = X2;  nxt->Y2 = Y2;
  nxt->BufSize  = (nxt->X2-nxt->X1+1) * (nxt->Y2-nxt->Y1+1) * 2;
  nxt->BufPointer = (char*) farmalloc((nxt->BufSize));
  WCheckPointer(nxt->BufPointer);

  _ClearWindow(nxt);
  wp=0;
  lo=(old->X2-old->X1+1)*2 ;
  ln=(nxt->X2-nxt->X1+1)*2 ;
  ho=(old->Y2-old->Y1)     ;
  hn=(nxt->Y2-nxt->Y1)     ;
  h =(ho > hn) ? hn : ho   ;
  l =(lo > ln) ? ln : lo   ;
  for (my=0; my<=h; ++my) {
    memcpy( (nxt->BufPointer+(my*ln)), (old->BufPointer+wp), l);
    wp+=lo;
  }
  free(old->BufPointer);
  free(old);

  WUnLock();

}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _CopyMainBufToScreen(ScrBuf bf){
cardinal Size,tmpB800;
  tmpB800=B800;
  Size=bf.Size/2;
  asm {    /* ds:si -> es:di */
    push  es; push ds; push si; push di; push cx;
  }
  _CX = FP_SEG(bf.Buf);
  asm push cx;
  _SI = FP_OFF(bf.Buf);
  asm{
    pop   ds
    xor   cx,cx
    mov   es,cx
    mov   di,es:[0x44E]         /* á¬¥é¥­¨¥  ªâ¨¢­®© ¢¨¤¥®áâà ­¨æë */
    mov   cx,tmpB800
    mov   es,cx
    mov   cx,Size
    cld
rep movsw
    pop   cx; pop di; pop si; pop ds; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _ChangeFullScreen(Coord X2, Coord Y2) {
  if (Y2<(43-1)) Y2=25-1;
  else if (Y2<(50-1)) Y2=43-1;
       else Y2=50-1;
  _Change(FullScreen,0,0,X2,Y2);
}
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _ReMakeScreenBuffer(ScrBuf* bfp) {
cardinal Len,Size;
  WLock();
  asm {
    push es; push ax;
    xor  ax,ax
    mov  es,ax
    mov  ax,es:[44Ch]           /* ¤«¨­  ¢¨¤¥®®¡« áâ¨   */
    mov  Size,ax
    mov  ax,es:[44Ah]           /* ¤«¨­  áâà®ª¨         */
    mov  Len,ax
    pop  ax; pop es;
  }
  if (bfp->Buf == NULL) {
    bfp->Size=Size;
    bfp->Len =Len;
    bfp->Heig=Size/Len/2;
    bfp->Buf =(char*) farmalloc((Size));
    WCheckPointer(bfp->Buf);
    memsetw(bfp->Buf,0x720,bfp->Size);
    _ChangeFullScreen(Len-1,bfp->Heig-1);
  }
  else {
    if (Size != bfp->Size) {
      free(bfp->Buf);
      bfp->Size=Size;
      bfp->Len =Len;
      bfp->Heig=Size/Len/2;
      bfp->Buf =(char*) farmalloc((Size));
      WCheckPointer(bfp->Buf);
      memsetw(bfp->Buf,0x720,bfp->Size);
      _ChangeFullScreen(Len-1,bfp->Heig-1);
    }
  WCheckPointer(bfp->Buf);
  }
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _CopyBufToMain(ScrBuf bf,WindType wt) {
cardinal mx,my,mp,wp,l,h,dwp;
  WCheckWindow(wt);
  if ((wt->X1 < bf.Len) && (wt->Y1 < (bf.Size/bf.Len))) {
    wp=0;
    h = (wt->Y2 > bf.Heig) ? bf.Heig : wt->Y2 ;
    l = (wt->X2 > bf.Len)  ? (bf.Len-wt->X1)*2 : (wt->X2-wt->X1+1)*2 ;
    dwp=(wt->X2-wt->X1+1)*2;
    for (my=wt->Y1; my<=h; ++my) {
      memcpy( (bf.Buf+((my*bf.Len)+wt->X1)*2), (wt->BufPointer+wp), l);
      wp+=dwp;
    }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _CopyBufToMainAndScreen(ScrBuf bf,WindType wt) {
cardinal mx,my,mp,wp,l,h,dwp,tmpB800;
WindType TopWT;
  tmpB800=B800;
  TopWT=WTop();
  WCheckWindow(wt);
  if ((wt->X1 < bf.Len) && (wt->Y1 < (bf.Size/bf.Len))) {
    wp=0;
    h = (wt->Y2 > bf.Heig) ? bf.Heig : wt->Y2 ;
    l = (wt->X2 > bf.Len)  ? (bf.Len-wt->X1)*2 : (wt->X2-wt->X1+1)*2 ;
    dwp=(wt->X2-wt->X1+1)*2;
    for (my=wt->Y1; my<=h; ++my) {
      mp=((my*bf.Len)+wt->X1)*2;
      memcpy( (bf.Buf+mp), (wt->BufPointer+wp), l);
      if (wt==TopWT) {
        asm {    /* ds:si -> es:di */
          push  es; push ds; push si; push di; push cx;
        }
        _CX = FP_SEG(wt->BufPointer+wp);
        asm push cx;
        _SI = FP_OFF(wt->BufPointer+wp);
        asm{
          pop   ds
          xor   cx,cx
          mov   es,cx
          mov   di,es:[0x44E]           /* á¬¥é¥­¨¥  ªâ¨¢­®© ¢¨¤¥®áâà ­¨æë */
          add   di,mp
          mov   cx,tmpB800
          mov   es,cx
          mov   cx,l
          sar   cx,1
          cld
      rep movsw
          pop   cx; pop di; pop si; pop ds; pop es;
        }
      }
      wp+=dwp;
    }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MainWrChar(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char C){
cardinal mp;
  mp=((Y*bf.Len)+X)*2;
  if (mp<bf.Size) {
    *(bf.Buf+mp)  =C;
    *(bf.Buf+mp+1)=(char)((B<<4)+F);
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MainWrCharRepX(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char C, cardinal R){
cardinal Leng;
  if (Y > bf.Heig) return;
  Leng = ((X+R) < bf.Len) ? R : (bf.Len-X);
  asm{    /* ax -> es:di */
    push  es; push di; push cx; push ax;
  }
  _DI = FP_OFF(bf.Buf)+(Y*bf.Len+X)*2;
  asm push di;
  _ES = FP_SEG(bf.Buf);
  asm{
    pop   di
    mov   ax,B
    mov   cl,4
    shl   ax,cl
    add   ax,F
    mov   ah,al
    mov   al,C
    mov   cx,Leng
    cld
rep stosw
    pop   ax; pop cx; pop di; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MainWrCharRepY(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char C, cardinal R){
cardinal Leng,L;
  if (Y > bf.Heig) return;
  Leng = ((Y+R) < bf.Heig) ? R : bf.Heig-Y;
  L=bf.Len*2-2;
  asm{    /* ax -> es:di */
    push  es; push di; push cx; push ax;
  }
  _DI = FP_OFF(bf.Buf)+(Y*bf.Len+X)*2;
  asm push di;
  _ES = FP_SEG(bf.Buf);
  asm{
    pop   di
    mov   ax,B
    mov   cl,4
    sal   ax,cl
    add   ax,F
    mov   ah,al
    mov   al,C
    mov   cx,Leng
    cld
  }
ckl:
  asm {
    stosw
    add   di,L
    loopne  ckl
    pop   ax; pop cx; pop di; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MainWrStr(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char *S){
cardinal Leng,i;
char     sym;
  if (Y > bf.Heig) return;
  Leng=strlen(S);
  if (Leng+X>bf.Len) Leng=bf.Len-X;
  i=0;
  asm{    /* ax -> es:di */
    push  es; push di; push cx; push ax;
  }
  _DI = FP_OFF(bf.Buf)+(Y*bf.Len+X)*2;
  asm push di;
  _ES = FP_SEG(bf.Buf);
  asm{
    pop   di
    mov   ax,B
    mov   cl,4
    sal   ax,cl
    add   ax,F
    mov   ah,al
    mov   cx,Leng
    cld
    push  es;
  }
cklmws:
  sym=S[i];
  ++i;
  asm {
    pop   es;
    mov   al,sym
    stosw
    push  es;
    loopne cklmws
    pop   es;
    pop   ax; pop cx; pop di; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MainWrStrN(ScrBuf bf, Coord X, Coord Y, Color F, Color B, char *S, unsigned int Numb){
cardinal Leng,i;
char     sym;
  if (Y > bf.Heig) return;
  Leng=strlen(S);
  if (Numb<Leng) Leng=Numb;
  if (Leng+X>bf.Len) Leng=bf.Len-X;
  i=0;
  asm{    /* ax -> es:di */
    push  es; push di; push cx; push ax;
  }
  _DI = FP_OFF(bf.Buf)+(Y*bf.Len+X)*2;
  asm push di;
  _ES = FP_SEG(bf.Buf);
  asm{
    pop   di
    mov   ax,B
    mov   cl,4
    sal   ax,cl
    add   ax,F
    mov   ah,al
    mov   cx,Leng
    cld
    push  es;
  }
cklmws:
  sym=S[i];
  ++i;
  asm {
    pop   es;
    mov   al,sym
    stosw
    push  es;
    loopne cklmws
    pop   es;
    pop   ax; pop cx; pop di; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _WriteFrame(ScrBuf bf,WindType wt) {
cardinal x,titlen,wtlen;
  WCheckWindow(wt);
  if ((wt->X1==0) || (wt->Y1==0)) return;
  wtlen=(wt->X2-wt->X1+1);
  _MainWrChar    (bf,wt->X1-1,wt->Y1-1,wt->FrameF,wt->FrameB,wt->FrameDef[0]);
  _MainWrCharRepX(bf,wt->X1,  wt->Y1-1,wt->FrameF,wt->FrameB,wt->FrameDef[1],wtlen);
  if (wt->UpperTitleType!=NoTitle) {
    titlen=strlen(wt->UpperTitleStr);
    if (titlen > wtlen) titlen=wtlen;
    switch (wt->UpperTitleType) {
      case LeftTitle   : x=wt->X1;
                         break;
      case CenterTitle : x=wt->X1+(wtlen-titlen)/2;
                         break;
      case RightTitle  : x=wt->X2+1-titlen;
    }
    _MainWrStrN(bf, x, wt->Y1-1, wt->FrameF, wt->FrameB, wt->UpperTitleStr, titlen);
  }
  if (wt->X2<=bf.Len) {
    _MainWrChar    (bf,wt->X2+1,wt->Y1-1,wt->FrameF,wt->FrameB,wt->FrameDef[2]);
    _MainWrCharRepY(bf,wt->X2+1,wt->Y1  ,wt->FrameF,wt->FrameB,wt->FrameDef[4],wt->Y2-wt->Y1+1);
    _MainWrChar    (bf,wt->X2+1,wt->Y2+1,wt->FrameF,wt->FrameB,wt->FrameDef[7]);
  }
  _MainWrCharRepX(bf,wt->X1,  wt->Y2+1,wt->FrameF,wt->FrameB,wt->FrameDef[6],wtlen);
  if (wt->LowerTitleType!=NoTitle) {
    titlen=strlen(wt->LowerTitleStr);
    if (titlen > wtlen) titlen=wtlen;
    switch (wt->LowerTitleType) {
      case LeftTitle   : x=wt->X1;
                         break;
      case CenterTitle : x=wt->X1+(wtlen-titlen)/2;
                         break;
      case RightTitle  : x=wt->X2+1-titlen;
    }
    _MainWrStrN(bf, x,  wt->Y2+1,wt->FrameF, wt->FrameB, wt->LowerTitleStr, titlen);
  }
  _MainWrChar    (bf,wt->X1-1,wt->Y2+1,wt->FrameF,wt->FrameB,wt->FrameDef[5]);
  _MainWrCharRepY(bf,wt->X1-1,wt->Y1  ,wt->FrameF,wt->FrameB,wt->FrameDef[3],wt->Y2-wt->Y1+1);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WCheckWindow(WindType wt) {
  if (wt->Guard!=GuardConst) {
    FatalError(" Invalid Window - Fatal ERROR ");
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WCheckPointer(void* bp) {
  if (bp==NULL) {
    FatalError(" Too small memory for Window buffer - Fatal ERROR ");
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType WFullAt(Coord X, Coord Y, AtMode *AtM) {
WindType wt,at;
int i;
  WLock();
  wt=FirstWindow;
  AtM=AtSpase;
  while (wt!=NULL) {
    WCheckWindow(wt);
    if (wt->HideOff==TRUE) {
      if ( (wt->X1<=X) && (X<=wt->X2) &&
           (wt->Y1<=Y) && (Y<=wt->Y2) ) { at=wt; *AtM=AtWindow; }
      else {
        if (wt->FrameOn) {
          if ( ((int)(wt->X1-1)<=(int)X) && (X<=(wt->X2+1)) &&
               ((int)(wt->Y1-1)<=(int)Y) && (Y<=(wt->Y2+1)) )  { at=wt; *AtM=AtFrame; };
        }
      }
    }
    wt=wt->Next;
  }
  WUnLock();
  return at;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType WAt(Coord X, Coord Y) {
WindType wt,at;
int i;
  WLock();
  wt=FirstWindow;
  while (wt!=NULL) {
    WCheckWindow(wt);
    if (wt->HideOff==TRUE) {
      if (wt->FrameOn) {
        if ( ((int)(wt->X1-1)<=(int)X) && (X<=(wt->X2+1)) &&
             ((int)(wt->Y1-1)<=(int)Y) && (Y<=(wt->Y2+1)) ) { at=wt; };
      }
      else {
        if ( (wt->X1<=X) && (X<=wt->X2) &&
             (wt->Y1<=Y) && (Y<=wt->Y2) ) { at=wt; };
      }
    }
    wt=wt->Next;
  }
  WUnLock();
  return at;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

boolean WIsFullAt(WindType wt, Coord X, Coord Y, AtMode *AtM) {
  WCheckWindow(wt);
  if (!wt->HideOff) return FALSE;

  WLock();
  if ( (wt->X1<=X) && (X<=wt->X2) &&
    (wt->Y1<=Y) && (Y<=wt->Y2) ) { *AtM=AtWindow; }
  else {
    if (wt->FrameOn) {
      if ( ((int)(wt->X1-1)<=(int)X) && (X<=wt->X2+1) &&
        ((int)(wt->Y1-1)<=(int)Y) && (Y<=wt->Y2+1) ) { *AtM=AtFrame; }
      else { WUnLock(); return FALSE; }
    }
    else { WUnLock(); return FALSE; }
  }

  wt=wt->Next;
  while (wt!=NULL) {
    WCheckWindow(wt);
    if (wt->HideOff==TRUE) {
      if ( (wt->X1<=X) && (X<=wt->X2) &&
           (wt->Y1<=Y) && (Y<=wt->Y2) ) { WUnLock(); return FALSE; }
      else {
        if (wt->FrameOn) {
          if ( ((int)(wt->X1-1)<=(int)X) && (X<=wt->X2+1) &&
               ((int)(wt->Y1-1)<=(int)Y) && (Y<=wt->Y2+1) ) { WUnLock(); return FALSE; }
        }
      }
    }
    wt=wt->Next;
  }
  WUnLock();
  return TRUE;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

boolean WIsAt(WindType wt, Coord X, Coord Y) {
  WCheckWindow(wt);
  if (!wt->HideOff) return FALSE;

  WLock();
  if ( (wt->X1<=X) && (X<=wt->X2) &&
    (wt->Y1<=Y) && (Y<=wt->Y2) ) {;}
  else {
    if (wt->FrameOn) {
      if ( ((int)(wt->X1-1)<=(int)X) && (X<=wt->X2+1) &&
        ((int)(wt->Y1-1)<=(int)Y) && (Y<=wt->Y2+1) ) {;}
      else { WUnLock(); return FALSE; }
    }
    else { WUnLock(); return FALSE; }
  }

  wt=wt->Next;
  while (wt!=NULL) {
    WCheckWindow(wt);
    if (wt->HideOff==TRUE) {
      if ( (wt->X1<=X) && (X<=wt->X2) &&
           (wt->Y1<=Y) && (Y<=wt->Y2) ) { WUnLock(); return FALSE; }
      else {
        if (wt->FrameOn)
          if ( ((int)(wt->X1-1)<=(int)X) && (X<=wt->X2+1) &&
               ((int)(wt->Y1-1)<=(int)Y) && (Y<=wt->Y2+1) ) { WUnLock(); return FALSE; }
      }
    }
    wt=wt->Next;
  }
  WUnLock();
  return TRUE;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _MakeCursor(void) {
WindType  Cwt;
char      x,y;
//  x=(char) CurrWT->X1+CurrWT->XC;
//  y=(char) CurrWT->Y1+CurrWT->YC;
//  if ((!(CurrWT->CursorOn)) || (!(CurrWT->HideOff)) || (WAt(x,y)!=CurrWT)) { y=25; };
  Cwt=CurrentWindow();
  x=(char) Cwt->X1+Cwt->XC;
  y=(char) Cwt->Y1+Cwt->YC;
  if ((!(Cwt->CursorOn)) || (!(Cwt->HideOff)) || (WAt(x,y)!=Cwt)) { y=25; };
  asm {
    push es; push ax; push bx; push dx;
    xor  ax,ax
    mov  es,ax
    mov  bh,es:[462h]
    mov  ah,2
    mov  dl,x
    mov  dh,y
    int  10h
    pop  dx; pop bx; pop ax; pop es;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReWriteAll(void) {
WindType wt;
Coord x,y;
int i;
  WLock();
  _ReMakeScreenBuffer(&MainScreenBuffer);
  memsetw(MainScreenBuffer.Buf,0x721,MainScreenBuffer.Size);
  wt=FirstWindow;
  while (wt!=NULL) {
    if (wt->HideOff) {
      _CopyBufToMain(MainScreenBuffer,wt);
      if (wt->FrameOn) _WriteFrame(MainScreenBuffer,wt);
    }
    wt=wt->Next;
  }
  if (WriteOn) {
    _CopyMainBufToScreen(MainScreenBuffer);
    _MakeCursor();
  }
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _ReWriteShort(WindType wtf) {
WindType wt;
Coord x,y;
int i;
  WLock();
  WCheckWindow(wtf);
  wt=wtf;
  while (wt!=NULL) {
    if (wt->HideOff) {
      _CopyBufToMain(MainScreenBuffer,wt);
      if (wt->FrameOn) _WriteFrame(MainScreenBuffer,wt);
    }
    wt=wt->Next;
  }
  if (WriteOn) {
    _CopyMainBufToScreen(MainScreenBuffer);
  }
  if (WriteOn) {
    _MakeCursor();
  }
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _CharToBuffer(WindType wt,char Sym) {
cardinal i;
  WCheckWindow(wt);
  i=(wt->X2-wt->X1+1)*wt->YC+wt->XC;
  if (i>=wt->BufSize) return;
  *(wt->BufPointer+i)  =Sym;
  *(wt->BufPointer+i+1)=(char) ((wt->Back<<4) + wt->Fore);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WFrameChange(WindType wt,FrameStr FrameDef)
{
  WCheckWindow(wt);
  strcpy(wt->FrameDef,FrameDef);
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WFrameON(WindType wt,
             Color FrameF, Color FrameB,
             FrameStr FrameDef)
{
  WCheckWindow(wt);
  wt->FrameOn  = TRUE;
  wt->FrameF   = FrameF;
  wt->FrameB   = FrameB;
  strcpy(wt->FrameDef,FrameDef);
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WFrameOFF(WindType wt) {
  WCheckWindow(wt);
  wt->FrameOn  = FALSE;
  if (wt->HideOff) WReWriteAll();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WUpperTitle(WindType wt, char *s, TitleMode tm) {
  WLock();
  WCheckWindow(wt);
  wt->UpperTitleType = tm;
  if (wt->UpperTitleStr!=NULL) free(wt->UpperTitleStr);
  if (tm!=NoTitle) {
    wt->UpperTitleStr  = (char*) farmalloc((strlen(s)+1));
    WCheckPointer(wt->UpperTitleStr);
    strcpy((wt->UpperTitleStr),s);
  }
  WUnLock();
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WUpperTitleOFF(WindType wt) {
  WLock();
  WCheckWindow(wt);
  wt->UpperTitleType = NoTitle;
  if (wt->UpperTitleStr!=NULL) free(wt->UpperTitleStr);
  WUnLock();
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WLowerTitle(WindType wt, char *s, TitleMode tm) {
  WLock();
  WCheckWindow(wt);
  wt->LowerTitleType = tm;
  if (wt->LowerTitleStr!=NULL) free(wt->LowerTitleStr);
  if (tm!=NoTitle) {
    wt->LowerTitleStr  = (char*) farmalloc((strlen(s)+1));
    WCheckPointer(wt->LowerTitleStr);
    strcpy((wt->LowerTitleStr),s);
  }
  WUnLock();
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WLowerTitleOFF(WindType wt) {
  WLock();
  WCheckWindow(wt);
  wt->LowerTitleType = NoTitle;
  if (wt->LowerTitleStr!=NULL) free(wt->LowerTitleStr);
  WUnLock();
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WClear(WindType wt) {
  WCheckWindow(wt);
  _ClearWindow(wt);
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WPutOnTop(WindType wt) {
  WCheckWindow(wt);
  _PutOnTop(wt);
//  if (wt->HideOff) WReWriteAll();
  if (wt->HideOff) _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WPutBeneath(WindType Twt, WindType Bwt) {
  WCheckWindow(Twt);
  WCheckWindow(Bwt);
  _PutBeneath(Twt,Bwt);
//  WReWriteAll();
  _ReWriteShort(Bwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType WTop(void) {
WindType wt;
int i;
  wt=FirstWindow;
  if (wt==NULL) return wt;
  loop {
    if (wt->Next==NULL) return wt;
    wt=wt->Next;
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType WOpen(WindDef* wd) {
WindType wt;
  WCheckWindow(FirstWindow);
  wt=_Open(wd);
//  if (wt->HideOff) WReWriteAll();
  if (wt->HideOff) _ReWriteShort(wt);
  return wt;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WChange(WindType nxt,Coord X1, Coord Y1, Coord X2, Coord Y2) {
  WCheckWindow(nxt);
  _Change(nxt,X1,Y1,X2,Y2);
  if (nxt->HideOff) WReWriteAll();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WClose(WindType wt){
WindType p1,p2;
  WLock();
  WCheckWindow(wt);
  if (FirstWindow==wt) FirstWindow=wt->Next;
  else {
    p1=FirstWindow;
    while (p1!=NULL) {
      WCheckWindow(p1);
      if (p1==wt) p2->Next=wt->Next;
      p2=p1;
      p1=p1->Next;
    }
  }
  if (wt==CurrentWindow()) SetCurrentWindow(WTop());
  CloseWindowUser(wt);
  if (wt->HideOff) WReWriteAll();
  free(wt->BufPointer);
  free(wt);
  if (wt->Guard==GuardConst) wt->Guard=0x1313;
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WUse(WindType wt) {
  WCheckWindow(wt);
//  CurrWT=wt;
  SetCurrentWindow(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

WindType WUsed(void) {
//  WCheckWindow(CurrWT);
//  return CurrWT;
  return CurrentWindow();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

char WGetWindowChar(WindType wt, Coord X, Coord Y, Color *Fore, Color *Back) {
cardinal i;
char f,b;
  WCheckWindow(wt);
  i=((wt->X2-wt->X1+1)*Y+X)*2;
  f=(*(wt->BufPointer+i));
  asm {
    push cx
    mov  ch,f
    mov  b,ch
    and  f,0x0F
    mov  cl,4
    sar  b,cl
    and  b,0x0F
    pop  cx
  }
  *Fore=(Color)f;
  *Back=(Color)b;
  return (*(wt->BufPointer+i+1));
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

char WGetScreenChar(Coord X, Coord Y, Color *Fore, Color *Back) {
cardinal i;
char f,b;
  i=(MainScreenBuffer.Len*Y+X)*2;
  f=(*(MainScreenBuffer.Buf+i));
  asm {
    push cx
    mov  ch,f
    mov  b,ch
    and  f,0x0F
    mov  cl,4
    sar  b,cl
    and  b,0x0F
    pop  cx
  }
  *Fore=(Color)f;
  *Back=(Color)b;
  return (*(MainScreenBuffer.Buf+i+1));
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WGotoXY(Coord X, Coord Y) {
WindType Cwt;
//  WCheckWindow(CurrWT);
//  CurrWT->XC=X;
//  CurrWT->YC=Y;
  Cwt=CurrentWindow();
  Cwt->XC=X;
  Cwt->YC=Y;
  _MakeCursor();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

Coord WhereX(void) {
//  WCheckWindow(CurrWT);
//  return CurrWT->XC;
  return CurrentWindow()->XC;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

Coord WhereY(void) {
//  WCheckWindow(CurrWT);
//  return CurrWT->YC;
  return CurrentWindow()->YC;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _ClrEol(Coord Y) {
cardinal l,h,i;
char     c;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  l=Cwt->X2-Cwt->X1+1;
  c=(char) ((Cwt->Back<<4)+Cwt->Fore);
  if (Y<(Cwt->Y2-Cwt->Y1+1)) {
    for (i=((Y*l)+Cwt->XC)*2; i<((Y+1)*l)*2; i+=2) {
      *(Cwt->BufPointer+i)  =' ';
      *(Cwt->BufPointer+i+1)=c  ;
    }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WInsLine(void) {
cardinal l,h,i;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  l=Cwt->X2-Cwt->X1+1;
  h=Cwt->Y2-Cwt->Y1+1;
  Cwt->XC=0;
  if (Cwt->YC<(Cwt->Y2-Cwt->Y1+1)) {
    i=(Cwt->YC+1)*l*2;
    memmove(Cwt->BufPointer+i, Cwt->BufPointer-l*2+i,(h-Cwt->YC-1)*2*l);
    _ClrEol(Cwt->YC);
    if (Cwt->HideOff) _ReWriteShort(Cwt);
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WDelLine(void) {
cardinal l,h,i,y;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  l=Cwt->X2-Cwt->X1+1;
  h=Cwt->Y2-Cwt->Y1+1;
  Cwt->XC=0;
  i=(Cwt->YC+1)*l*2;
  if (Cwt->YC<(Cwt->Y2-Cwt->Y1+1)) {
    memmove(Cwt->BufPointer-l*2+i, Cwt->BufPointer+i,(h-Cwt->YC-1)*2*l);
    _ClrEol(h-1);
    if (Cwt->HideOff) _ReWriteShort(Cwt);
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WSetColors(Color Fore, Color Back) {
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  Cwt->Fore=Fore;
  Cwt->Back=Back;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColor(Color OldFore, Color OldBack, Color NewFore, Color NewBack) {
cardinal Len;
char     Co,Cn;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  Co=(char) ((OldBack<<4)+OldFore);
  Cn=(char) ((NewBack<<4)+NewFore);
  Len=Cwt->BufSize/2;
  asm{    /* ax <- ds:si */
    push  ds; push si; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    mov   cx,Len
    cld
  }
cklrcf:
  asm {
    lodsw
    cmp   ah,Co
    jne   notrcf
    mov   ah,Cn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
notrcf:
  asm {
    loopne  cklrcf
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColorFore(Color OldFore, Color NewFore) {
cardinal Len;
char     Fo,Fn;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  Fo=(char) (OldFore);
  Fn=(char) (NewFore);
  Len=Cwt->BufSize/2;
  asm{    /* ax <- ds:si */
    push  ds; push si; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    mov   cx,Len
    cld
  }
ckwrcf:
  asm {
    lodsw
    mov   dh,ah
    and   dh,0x0F
    cmp   dh,Fo
    jne   nowrcf
    and   ah,0xF0
    or    ah,Fn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
nowrcf:
  asm {
    loopne  ckwrcf
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColorBack(Color OldBack, Color NewBack) {
cardinal Len;
char     Bo,Bn;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  Bo=(char) (OldBack<<4);
  Bn=(char) (NewBack<<4);
  Len=Cwt->BufSize/2;
  asm{    /* ax <- ds:si */
    push  ds; push si; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    mov   cx,Len
    cld
  }
ckwrcb:
  asm {
    lodsw
    mov   dh,ah
    and   dh,0xF0
    cmp   dh,Bo
    jne   nowrcb
    and   ah,0x0F
    or    ah,Bn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
nowrcb:
  asm {
    loopne  ckwrcb
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColorLine(Coord X, Coord Y, int Len,
                  Color OldFore, Color OldBack,
                  Color NewFore, Color NewBack) {
cardinal Offs;
char     Co,Cn,l;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  if ((X>Cwt->X2) || (Y>Cwt->Y2))return;
  l=Cwt->X2-Cwt->X1+1;
  Offs=(l*Y+X)*2;//+1;
  Co=(char) ((OldBack<<4)+OldFore);
  Cn=(char) ((NewBack<<4)+NewFore);
  if ((X+Len) > l) Len=l-X;
  asm{    /* ax <- ds:si */
    push  ds; push di; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    add   si,Offs
    mov   cx,Len
    cld
  }
cklrcl:
  asm {
    lodsw
    cmp   ah,Co
    jne   notrcl
    mov   ah,Cn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
notrcl:
  asm {
    loopne  cklrcl
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColorLineBack(Coord X, Coord Y, int Len,
                      Color OldBack, Color NewBack) {
cardinal Offs;
char     Bo,Bn,l;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  if ((X>Cwt->X2) || (Y>Cwt->Y2))return;
  l=Cwt->X2-Cwt->X1+1;
  Offs=(l*Y+X)*2;//+1;
  Bo=(char) (OldBack<<4);
  Bn=(char) (NewBack<<4);
  if ((X+Len) > l) Len=l-X;
  asm{    /* ax <- ds:si */
    push  ds; push di; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    add   si,Offs
    mov   cx,Len
    cld
  }
cklrcl:
  asm {
    lodsw
    mov   dh,ah
    and   dh,0xF0
    cmp   dh,Bo
    jne   notrcl
    and   ah,0x0F
    or    ah,Bn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
notrcl:
  asm {
    loopne  cklrcl
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WReColorLineFore(Coord X, Coord Y, int Len,
                      Color OldFore, Color NewFore) {
cardinal Offs;
char     Fo,Fn,l;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  if ((X>Cwt->X2) || (Y>Cwt->Y2))return;
  l=Cwt->X2-Cwt->X1+1;
  Offs=(l*Y+X)*2;//+1;
  Fo=(char) (OldFore);
  Fn=(char) (NewFore);
  if ((X+Len) > l) Len=l-X;
  asm{    /* ax <- ds:si */
    push  ds; push di; push cx; push ax; push dx;
  }
  _CX = FP_SEG(Cwt->BufPointer);
  asm push cx;
  _SI = FP_OFF(Cwt->BufPointer);
  asm{
    pop   ds
    add   si,Offs
    mov   cx,Len
    cld
  }
cklrcl:
  asm {
    lodsw
    mov   dh,ah
    and   dh,0x0F
    cmp   dh,Fo
    jne   notrcl
    and   ah,0xF0
    or    ah,Fn
    dec   si
    mov   ds:[si],ah
    inc   si
  }
notrcl:
  asm {
    loopne  cklrcl
    pop   dx; pop ax; pop cx; pop si; pop ds;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WSetColorsLine(Coord X, Coord Y, int Len, Color Fore, Color Back) {
cardinal Offs;
char     Cn,l;
WindType Cwt;
//  WCheckWindow(CurrWT);
  Cwt=CurrentWindow();
  if ((X>Cwt->X2) || (Y>Cwt->Y2))return;
  l=Cwt->X2-Cwt->X1+1;
  Offs=(l*Y+X)*2+1;
  Cn=(char) ((Back<<4)+Fore);
  if ((X+Len) > l) Len=l-X;
  asm{    /* ax -> es:di */
    push  ds; push di; push cx; push ax; push dx;
  }
  _ES = FP_SEG(Cwt->BufPointer);
  asm push es;
  _DI = FP_OFF(Cwt->BufPointer);
  asm{    /* ax -> ds:si */
    pop   es
    add   di,Offs
    mov   al,Cn
    mov   cx,Len
    cld
  }
cklscl:
  asm {
    stosb
    inc di
    loopne  cklscl
    pop   dx; pop ax; pop cx; pop di; pop es;
  }
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrapON(void){
//  WCheckWindow(CurrWT);
//  CurrWT->WrapOn=TRUE;
  CurrentWindow()->WrapOn=TRUE;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrapOFF(void){
//  WCheckWindow(CurrWT);
//  CurrWT->WrapOn=FALSE;
  CurrentWindow()->WrapOn=FALSE;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WHideON(WindType wt){
  WCheckWindow(wt);
  wt->HideOff=FALSE;
  WReWriteAll();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WHideOFF(WindType wt){
  WCheckWindow(wt);
  wt->HideOff=TRUE;
//  WReWriteAll();
  _ReWriteShort(wt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WCursorON(void) {
//  WCheckWindow(CurrWT);
//  CurrWT->CursorOn=TRUE;
  CurrentWindow()->CursorOn=TRUE;
  _MakeCursor();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WCursorOFF(void) {
//  WCheckWindow(CurrWT);
//  CurrWT->CursorOn=FALSE;
  CurrentWindow()->CursorOn=FALSE;
  _MakeCursor();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

boolean WWriteON(void) {
boolean wo;
  wo=WriteOn;
  WriteOn=TRUE;
  WReWriteAll();
  return wo;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

boolean WWriteOFF(void) {
boolean wo;
  wo=WriteOn;
  WriteOn=FALSE;
  return wo;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WClrEol(void) {
WindType Cwt;
  Cwt=CurrentWindow();
  _ClrEol(Cwt->YC);
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _WrLn(void) {
cardinal l,h,i;
WindType Cwt;
  Cwt=CurrentWindow();
  l=Cwt->X2-Cwt->X1+1;
  h=Cwt->Y2-Cwt->Y1+1;
  if (Cwt->YC < (h-1)) {
    Cwt->XC=0; ++(Cwt->YC);
  }
  else {
    memmove(Cwt->BufPointer, Cwt->BufPointer+l*2,(h-1)*l*2);
    Cwt->XC=0;
    _ClrEol(Cwt->YC);
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrLn(void) {
WindType Cwt;
  Cwt=CurrentWindow();
  _WrLn();
  if (Cwt->HideOff) _ReWriteShort(Cwt);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _SlowSetChar(WindType wt, Coord X, Coord Y, char C){
cardinal l,h,p;
  WCheckWindow(wt);
  l=wt->X2-wt->X1+1;
  h=wt->Y2-wt->Y1+1;
  if (X<l) {
    if (Y<h) {
      p=(l*Y+X)*2;
      *(wt->BufPointer+p)  =C;
      *(wt->BufPointer+p+1)=(char) ((wt->Back<<4)+wt->Fore);
    }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _SetChar(WindType wt, Coord X, Coord Y, char C){
cardinal l,h,p,tmpB800;
cardinal mp;
char     c;
  tmpB800=B800;
  WCheckWindow(wt);
  l=wt->X2-wt->X1+1;
  h=wt->Y2-wt->Y1+1;
  c=(char) ((wt->Back<<4)+wt->Fore);
  if ((X<l) && (Y<h)) {
    p=(l*Y+X)*2;
    *(wt->BufPointer+p)  =C;
    *(wt->BufPointer+p+1)=c;
//  if ((wt->HideOff) && (wt==WAt(wt->X1+X,wt->Y1+Y))) {
    if ((wt->HideOff) && (WIsAt(wt,(wt->X1+X),(wt->Y1+Y)))) {
      if ((wt->X1 < MainScreenBuffer.Len) && (wt->Y1 < (MainScreenBuffer.Size/MainScreenBuffer.Len))) {
        mp=(((wt->Y1+Y)*MainScreenBuffer.Len)+wt->X1+X)*2;
        *(MainScreenBuffer.Buf+mp)  =C;
        *(MainScreenBuffer.Buf+mp+1)=c;
        if (WriteOn) {
//        if (wt==WAt((wt->X1+X),(wt->Y1+Y))) {
          if (WIsAt(wt,(wt->X1+X),(wt->Y1+Y))) {
            asm {
              push  es; push bx; push cx;
              xor   cx,cx
              mov   es,cx
              mov   bx,es:[0x44E]               /* á¬¥é¥­¨¥  ªâ¨¢­®© ¢¨¤¥®áâà ­¨æë */
              add   bx,mp
              mov   cx,tmpB800
              mov   es,cx
              mov   cl,C
              mov   es:[bx],cl
              inc   bx
              mov   cl,c
              mov   byte ptr es:[bx],cl
              pop   cx; pop bx; pop es;
            }
          }
        }
      }
    }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WSetChar(WindType wt, Coord X, Coord Y, char C){
  WCheckWindow(wt);
  _SetChar(wt, X, Y, C);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _WrChar(char C) {
cardinal l,h,x;
WindType Cwt;
  Cwt=CurrentWindow();
  l=Cwt->X2-Cwt->X1+1;
  switch (C) {
    case 007 : Beep();                                          break;
    case 011 : x=(((Cwt->XC/8)+1)*8);
               while (Cwt->XC < x)  _WrChar(' ');               break;
    case 012 : x=Cwt->XC; WrLn(); Cwt->XC=x;                    break;
    case 014 : _ClearWindow(Cwt); WGotoXY(0,0);                 break;
    case 015 : Cwt->XC=0;                                       break;
    default  : _SetChar(Cwt,Cwt->XC++,Cwt->YC,C);
               if (Cwt->XC >= l) {
                 if (Cwt->WrapOn) {
                   WrLn();
                 }
               }
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrChar(char C) {
  _WrChar(C);
  _MakeCursor();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrStr(char *S) {
  while (*S!=0) {
    _WrChar(*S);
    ++S;
  }
  _MakeCursor();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void _StrLineUp(char *s, int len) {
int delta,l;
char spchr;
  l=strlen(s);
  if (len>=0) spchr=' ';
  else {
    spchr='0';
    if (*s=='-') {
      s++;
      l--;
    }
  }
  len=abs(len);
  delta=len-l;
  if (delta>0) {
    memmove((s+delta),s,l+1);
    memset(s,spchr,delta);
  }
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrIntBase(int i , int l, int base) {
char s[40];
  itoa(i,s,base);
  _StrLineUp(s,l);
  WrStr(s);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrLngIntBase(long int i , int l, int base) {
char s[40];
cardinal n,k;
  ltoa(i,s,base);
  _StrLineUp(s,l);
  WrStr(s);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrUnsBase(unsigned int i , int l, int base) {
char s[40];
long unsigned int u;
  u=(long unsigned int ) i;
  ultoa(u,s,base);
  _StrLineUp(s,l);
  WrStr(s);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrLngUnsBase(long unsigned int i , int l, int base) {
char s[40];
  ultoa(i,s,base);
  _StrLineUp(s,l);
  WrStr(s);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrInt(int i, int len) {
  WrIntBase(i,len,10);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrLngInt(long int i, int len) {
  WrLngIntBase(i,len,10);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrUns(unsigned int i , int len) {
  WrUnsBase(i,len,10);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrLngUns(long unsigned int i , int len) {
  WrLngUnsBase(i,len,10);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WrFloat(float f, int len, int prec ) {
char    S[40],format[10];
boolean ok;
int     l,i;
  strcpy(format,"%");
  itoa(len,S,10);
  strcat(format,S);
  strcat(format,".");
  itoa(prec,S,10);
  strcat(format,S);
  strcat(format,"f");
  S[0]=0;
  sprintf(S,format,f);
  i=(len+prec+1)-strlen(S);
  if (i>0) {
    for (; i>0; i--) {
      strincl(S," ");
    }
  }
  WrStr(S);
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WSaveScreen(ScrBuf *bf) {
cardinal Size,tmpB800;
  tmpB800=B800;
  WLock();
  bf->Size = MainScreenBuffer.Size;
  bf->Len  = MainScreenBuffer.Len;
  bf->Heig = MainScreenBuffer.Heig;
  bf->Buf  = (char*)farmalloc((bf->Size));
  WCheckPointer(bf->Buf);
  Size     = bf->Size/2;
  asm{    /* ds:si -> es:di */
    push  es; push ds; push si; push di; push cx;
  }
  _ES = FP_SEG(bf->Buf);
  asm push es;
  _DI = FP_OFF(bf->Buf);
  asm{    /* ds:si -> es:di */
    pop   es
    xor   cx,cx
    mov   ds,cx
    mov   si,ds:[0x44E]         /* á¬¥é¥­¨¥  ªâ¨¢­®© ¢¨¤¥®áâà ­¨æë */
    mov   cx,tmpB800
    mov   ds,cx
    mov   cx,Size
    cld
rep movsw
    pop   cx; pop di; pop si; pop ds; pop es;
  }
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void WRestoreScreen(ScrBuf *bf) {
  if (bf->Buf==NULL) return;
  WLock();
  _CopyMainBufToScreen(*bf);
  free(bf->Buf);
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void InitWindowLibrary(void) {
cardinal Size,tmpB800;
  WLock();
  asm {
    push  ds;   push bx;
    xor   bx,bx
    mov   ds,bx
    mov   bx,449h
    mov   tmpB800,0xB800
    cmp   byte ptr ds:[bx],7
    jne   StdMemAddr
    mov   tmpB800,0xB000
  }
StdMemAddr:
  asm{
    pop  bx;   pop  ds;
  }
  B800=tmpB800;
  FullScreen=_Open(&FullScreenDef);
  Size     = FullScreen->BufSize/2;
  asm{    /* ds:si -> es:di */
    push  es; push ds; push si; push di; push cx;
  }
  _ES = FP_SEG(FullScreen->BufPointer);
  asm push es;
  _DI = FP_OFF(FullScreen->BufPointer);
  asm{    /* ds:si -> es:di */
    pop   es
    xor   cx,cx
    mov   ds,cx
    mov   si,ds:[0x44E]         /* á¬¥é¥­¨¥  ªâ¨¢­®© ¢¨¤¥®áâà ­¨æë */
    mov   cx,tmpB800
    mov   ds,cx
    mov   cx,Size
    cld
rep movsw
    pop   cx; pop di; pop si; pop ds; pop es;
  }
  _ReMakeScreenBuffer(&MainScreenBuffer);
  FirstWindow->WrapOn=TRUE;
  WReWriteAll();
  WUnLock();
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

void SetWindowMultiProcess(void  (*LockProcess)(),
                           void  (*UnLockProcess)(),
                           long  CP){
//                         void* (*CurrentProcess)()){
  MultiP   = TRUE;
  WLock    = LockProcess;
  WUnLock  = UnLockProcess;
  CurrProc = (long) CP; //CurrentProcess;
}

/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */





/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */
