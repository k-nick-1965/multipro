/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ *\
			      window.h

			   Window library

			 remembe TS Modula-2

			(C)  Kaling N.V. 1993

\* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */
#if     !defined(__WINDOW_H)
#define __WINDOW_H

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#ifndef NULL
#if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#define NULL    0
#else
#define NULL    0L
#endif
#endif

/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */

#define SingleFrame "ÚÄ¿³³ÀÄÙ"
#define	DoubleFrame "ÉÍ»ººÈÍ¼"

#if     !defined(TRUE)
typedef int boolean;
#define	TRUE  ((boolean) 1)
#define FALSE ((boolean) 0)
#define loop while (0==0)
#endif

#define Coord cardinal


/* ÄÄÄ typedef ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

typedef unsigned int cardinal;

typedef enum       { Black   = 0,Blue        = 1,Green     = 2,Cyan     = 3,
		     Red     = 4,Magenta     = 5,Brown     = 6,LightGray= 7,
		     DarkGray= 8,LightBlue   = 9,LightGreen=10,LightCyan=11,
		     LightRed=12,LightMagenta=13,Yellow    =14,White    =15
		   } Color;

typedef enum       { NoTitle,
		     LeftTitle,CenterTitle,RightTitle
		   } TitleMode ;

typedef enum       { AtSpase, AtWindow, AtFrame
		   } AtMode ;

typedef char         FrameStr[9];   		/* 0 1 2 */
						/* 3   4 */
						/* 5 6 7 */
typedef struct     { Coord X1, Y1;     		/* ¢¥àå­¨© «¥¢ë© ã£®«  X=0-79 */
		     Coord X2, Y2; 		/* ­¨¦­¨© ¯à ¢ë© ã£®«  Y=0-24 */
		     boolean FrameOn;       	/* ­ «¨ç¨¥ à ¬ª¨	      */
		     boolean HideOff;		/* ¯à¨áãâáâ¢¨¥ ­  íªà ­¥      */
		     Color Fore, Back;		/* æ¢¥â â¥ªáâ   	      */
		     Color FrameF, FrameB;	/* æ¢¥â  à ¬ª¨ 		      */
		     FrameStr FrameDef;		/* á¨¬¢®«ë à ¬ª¨              */
		   } WindDef ;
// à ¬ª  ®¯¨áë¢ ¥âáï ¢®ªàã£ ®ª­  § ¤ ­­®£® ¢ WindDef, â.¥. çâ®¡ë à ¬ª 
// ¯®¬¥áâ¨« áì ­  íªà ­¥ ®ª­® ¤.¡. á¤¢¨­ãâ® ­  1 ®â ¢á¥å ªà ¥¢ íªà ­ .

typedef struct     { cardinal Size;
		     char *Point;
		   } ScreenType;

typedef struct     { cardinal  Size;
		     cardinal  Len;
		     cardinal  Heig;
		     char* Buf;
		   } ScrBuf;

struct MainWindDef { cardinal Guard;		/* ¯à¨§­ ª ®ª­  	    */
		     struct MainWindDef *Next;	/* á«¥¤ãîé¨© 		    */
		     Coord X1, Y1; 		/* ¢¥àå­¨© «¥¢ë© ã£®«(íªà ­)*/
		     Coord X2, Y2; 		/* ­¨¦­¨© ¯à ¢ë© ã£®«(íªà ­)*/
		     Coord XC, YC;	 	/* ¯®«®¦¥­¨¥ ªãàá®à  ( ®ª­®)*/
		     boolean CursorOn;   	/* ­ «¨ç¨¥ ªãàá®à 	    */
		     boolean FrameOn;       	/* ­ «¨ç¨¥ à ¬ª¨	    */
		     boolean HideOff;		/* ¯à¨áãâáâ¢¨¥ ­  íªà ­¥    */
		     boolean WrapOn;		/* ¯¥à¥å®¤ ­  £à ­¨æ¥	    */
		     Color Fore, Back;		/* æ¢¥â â¥ªáâ   	    */
		     Color FrameF, FrameB;	/* æ¢¥â  à ¬ª¨ 		    */
		     FrameStr FrameDef;		/* á¨¬¢®«®ë à ¬ª¨           */
		     TitleMode UpperTitleType;	/* â¨¯ ¢¥àå­¥£® â¨âã« 	    */
		     char *UpperTitleStr;	/* ¢¥àå­¨© â¨âã«	    */
		     TitleMode LowerTitleType;	/* â¨¯ ­¨¦­¥£® â¨âã« 	    */
		     char *LowerTitleStr;	/* ­¨¦­¨© â¨âã«             */
		     cardinal BufSize;		/* à §¬¥à ¡ãä¥à  åà ­¥­¨ï   */
		     char* BufPointer;  	/* ãª § â¥«ì ­  ¡ãä¥à 	    */
		   } ;

typedef struct MainWindDef MainWindDefType;

typedef struct MainWindDef *WindType;

/* ÄÄÄ Var ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern         WindType    FullScreen;

/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */

#ifdef __cplusplus
extern "C" {
#endif

/* ÄÄÄ Initialization ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void InitWindowLibrary() ;
extern  void SetWindowMultiProcess(void  (*LockProcess)(),
				   void  (*UnLockProcess)(),
				   long  CP);
//				   void* (*CurrentProcess)());

/* ÄÄÄ General ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  FatalError(char *s) ;
extern	void	  _Cdecl  Beep(void) ;

/* ÄÄÄ Window ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	WindType  _Cdecl  WOpen(WindDef* wd) ;
extern	void      _Cdecl  WClose(WindType wt);

extern	void      _Cdecl  WUse(WindType wt) ;
extern	void      _Cdecl  WPutOnTop(WindType wt) ;
extern	void      _Cdecl  WPutBeneath(WindType Twt, WindType Bwt) ;

extern	void      _Cdecl  WChange(WindType nxt,Coord X1, Coord Y1, Coord X2, Coord Y2) ;

extern	WindType  _Cdecl  WUsed(void) ;
extern	WindType  _Cdecl  WTop(void) ;

extern	WindType  _Cdecl  WFullAt(Coord X, Coord Y, AtMode *AtM) ;
extern	WindType  _Cdecl  WAt(Coord X, Coord Y) ;
extern	boolean   _Cdecl  WIsFullAt(WindType wt, Coord X, Coord Y, AtMode *AtM) ;
extern	boolean   _Cdecl  WIsAt(WindType wt, Coord X, Coord Y) ;

/* ÄÄÄ Window Screen Write ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WReWriteAll(void) ;
extern	boolean   _Cdecl  WWriteON(void) ;
extern	boolean   _Cdecl  WWriteOFF(void) ;

/* ÄÄÄ Window Check ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WCheckWindow(WindType wt) ;
extern	void      _Cdecl  WCheckPointer(void* bp) ;

/* ÄÄÄ Window Colors ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WSetColors(Color Fore, Color Back) ;
extern	void      _Cdecl  WReColor(Color OldFore, Color OldBack, Color NewFore, Color NewBack) ;
extern	void      _Cdecl  WReColorBack(Color OldBack, Color NewBack) ;
extern	void      _Cdecl  WReColorFore(Color OldFore, Color NewFore) ;

/* ÄÄÄ Window Line Colors ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WSetColorsLine(Coord X, Coord Y ,int Len, Color Fore, Color Back );
extern	void      _Cdecl  WReColorLine(Coord X, Coord Y, int Len,
				       Color OldFore, Color OldBack,
				       Color NewFore, Color NewBack) ;
extern	void      _Cdecl  WReColorLineBack(Coord X, Coord Y, int Len,
					   Color OldBack, Color NewBack) ;
extern	void      _Cdecl  WReColorLineFore(Coord X, Coord Y, int Len,
					   Color OldFore, Color NewFore) ;

/* ÄÄÄ Window Frame & Titles ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void 	  _Cdecl  WFrameChange(WindType wt,FrameStr FrameDef);
extern	void      _Cdecl  WFrameON(WindType wt, Color FrameF, Color FrameB, FrameStr FrameDef) ;
extern	void      _Cdecl  WFrameOFF(WindType wt) ;

extern	void      _Cdecl  WUpperTitle(WindType wt, char *s, TitleMode tm) ;
extern	void      _Cdecl  WUpperTitleOFF(WindType wt) ;
extern	void      _Cdecl  WLowerTitle(WindType wt, char *s, TitleMode tm) ;
extern	void      _Cdecl  WLowerTitleOFF(WindType wt) ;

/* ÄÄÄ Window Wrap ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WrapON(void);
extern	void      _Cdecl  WrapOFF(void);

/* ÄÄÄ Window Hide ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WHideON(WindType wt);
extern	void      _Cdecl  WHideOFF(WindType wt);

/* ÄÄÄ Window Cursor ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WCursorON(void) ;
extern	void      _Cdecl  WCursorOFF(void) ;

extern	Coord     _Cdecl  WhereX(void) ;
extern	Coord     _Cdecl  WhereY(void) ;

extern	void      _Cdecl  WGotoXY(Coord X, Coord Y) ;

/* ÄÄÄ Window Line ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WInsLine(void) ;
extern	void      _Cdecl  WDelLine(void) ;
extern	void      _Cdecl  WClrEol(void) ;

/* ÄÄÄ Window Write ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WClear(WindType wt) ;

extern	void      _Cdecl  WrLn(void) ;

extern	void      _Cdecl  WSetChar(WindType wt, Coord X, Coord Y, char C);
extern	void      _Cdecl  WrChar(char C) ;
extern	void      _Cdecl  WrStr(char *S) ;

extern	void      _Cdecl  WrIntBase(int i , int l, int base) ;
extern	void      _Cdecl  WrLngIntBase(long int i , int l, int base) ;
extern	void      _Cdecl  WrUnsBase(unsigned int i , int l, int base) ;
extern	void      _Cdecl  WrlongUnsBase(long unsigned int i , int l, int base) ;

extern	void      _Cdecl  WrInt(int i, int len) ;
extern	void      _Cdecl  WrLngInt(long int i, int len) ;
extern	void      _Cdecl  WrUns(unsigned int i , int len) ;
extern	void      _Cdecl  WrLngUns(long unsigned int i , int len) ;

extern	void      _Cdecl  WrFloat(float f, int before, int after ) ;

// ¥á«¨ l ¨«¨ len <0, â® «¨¤¨àãîé¨¥ 0-¨, ¨­ ç¥ - ¯à®¡¥«ë.

/* ÄÄÄ Window Get ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	char      _Cdecl  WGetWindowChar(WindType wt, Coord X, Coord Y, Color *Fore, Color *Back) ;
extern	char      _Cdecl  WGetScreenChar(Coord X, Coord Y, Color *Fore, Color *Back) ;

/* ÄÄÄ Screen ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */

extern	void      _Cdecl  WSaveScreen(ScrBuf *bf) ;
extern	void      _Cdecl  WRestoreScreen(ScrBuf *bf) ;

/* ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ */

#ifdef __cplusplus
}
#endif

#endif  /* __WINDOW_H */
