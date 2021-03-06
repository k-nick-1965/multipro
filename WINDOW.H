/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 *\
			      window.h

			   Window library

			 remembe TS Modula-2

			(C)  Kaling N.V. 1993

\* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */
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

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#define SingleFrame "敖審垣潰"
#define	DoubleFrame "浜産哉夕"

#if     !defined(TRUE)
typedef int boolean;
#define	TRUE  ((boolean) 1)
#define FALSE ((boolean) 0)
#define loop while (0==0)
#endif

#define Coord cardinal


/* 陳� typedef 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

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
typedef struct     { Coord X1, Y1;     		/* ▲璢�┤ �ア覃 磽��  X=0-79 */
		     Coord X2, Y2; 		/* �└�┤ ����覃 磽��  Y=0-24 */
		     boolean FrameOn;       	/* ����腮� �����	      */
		     boolean HideOff;		/* �爬痺矚癶┘ �� 蹣����      */
		     Color Fore, Back;		/* 罌モ 皀�痰�  	      */
		     Color FrameF, FrameB;	/* 罌モ� ����� 		      */
		     FrameStr FrameDef;		/* 瓱�〓�� �����              */
		   } WindDef ;
// ����� ���瘠��モ瘴 〓�珮� ���� ��������� � WindDef, �.�. 艪�° �����
// ���メ皋��瘡 �� 蹣���� ���� �.�. 瓩※�竄� �� 1 �� ≡ュ ���ア 蹣����.

typedef struct     { cardinal Size;
		     char *Point;
		   } ScreenType;

typedef struct     { cardinal  Size;
		     cardinal  Len;
		     cardinal  Heig;
		     char* Buf;
		   } ScrBuf;

struct MainWindDef { cardinal Guard;		/* �爬Л�� ���� 	    */
		     struct MainWindDef *Next;	/* 甄イ竡薑� 		    */
		     Coord X1, Y1; 		/* ▲璢�┤ �ア覃 磽��(蹣���)*/
		     Coord X2, Y2; 		/* �└�┤ ����覃 磽��(蹣���)*/
		     Coord XC, YC;	 	/* ����Ε�┘ �窶甌�� ( ����)*/
		     boolean CursorOn;   	/* ����腮� �窶甌��	    */
		     boolean FrameOn;       	/* ����腮� �����	    */
		     boolean HideOff;		/* �爬痺矚癶┘ �� 蹣����    */
		     boolean WrapOn;		/* �ムュ�� �� �����罐	    */
		     Color Fore, Back;		/* 罌モ 皀�痰�  	    */
		     Color FrameF, FrameB;	/* 罌モ� ����� 		    */
		     FrameStr FrameDef;		/* 瓱�〓��� �����           */
		     TitleMode UpperTitleType;	/* 皋� ▲璢�ィ� 皋矣��	    */
		     char *UpperTitleStr;	/* ▲璢�┤ 皋矣�	    */
		     TitleMode LowerTitleType;	/* 皋� �└�ィ� 皋矣��	    */
		     char *LowerTitleStr;	/* �└�┤ 皋矣�             */
		     cardinal BufSize;		/* ��Кム ＜筌�� 縲��キ��   */
		     char* BufPointer;  	/* 礫���皀�� �� ＜筌� 	    */
		   } ;

typedef struct MainWindDef MainWindDefType;

typedef struct MainWindDef *WindType;

/* 陳� Var 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern         WindType    FullScreen;

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
extern "C" {
#endif

/* 陳� Initialization 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

extern	void InitWindowLibrary() ;
extern  void SetWindowMultiProcess(void  (*LockProcess)(),
				   void  (*UnLockProcess)(),
				   long  CP);
//				   void* (*CurrentProcess)());

/* 陳� General 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  FatalError(char *s) ;
extern	void	  _Cdecl  Beep(void) ;

/* 陳� Window 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

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

/* 陳� Window Screen Write 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WReWriteAll(void) ;
extern	boolean   _Cdecl  WWriteON(void) ;
extern	boolean   _Cdecl  WWriteOFF(void) ;

/* 陳� Window Check 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

extern	void      _Cdecl  WCheckWindow(WindType wt) ;
extern	void      _Cdecl  WCheckPointer(void* bp) ;

/* 陳� Window Colors 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WSetColors(Color Fore, Color Back) ;
extern	void      _Cdecl  WReColor(Color OldFore, Color OldBack, Color NewFore, Color NewBack) ;
extern	void      _Cdecl  WReColorBack(Color OldBack, Color NewBack) ;
extern	void      _Cdecl  WReColorFore(Color OldFore, Color NewFore) ;

/* 陳� Window Line Colors 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

extern	void      _Cdecl  WSetColorsLine(Coord X, Coord Y ,int Len, Color Fore, Color Back );
extern	void      _Cdecl  WReColorLine(Coord X, Coord Y, int Len,
				       Color OldFore, Color OldBack,
				       Color NewFore, Color NewBack) ;
extern	void      _Cdecl  WReColorLineBack(Coord X, Coord Y, int Len,
					   Color OldBack, Color NewBack) ;
extern	void      _Cdecl  WReColorLineFore(Coord X, Coord Y, int Len,
					   Color OldFore, Color NewFore) ;

/* 陳� Window Frame & Titles 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void 	  _Cdecl  WFrameChange(WindType wt,FrameStr FrameDef);
extern	void      _Cdecl  WFrameON(WindType wt, Color FrameF, Color FrameB, FrameStr FrameDef) ;
extern	void      _Cdecl  WFrameOFF(WindType wt) ;

extern	void      _Cdecl  WUpperTitle(WindType wt, char *s, TitleMode tm) ;
extern	void      _Cdecl  WUpperTitleOFF(WindType wt) ;
extern	void      _Cdecl  WLowerTitle(WindType wt, char *s, TitleMode tm) ;
extern	void      _Cdecl  WLowerTitleOFF(WindType wt) ;

/* 陳� Window Wrap 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WrapON(void);
extern	void      _Cdecl  WrapOFF(void);

/* 陳� Window Hide 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WHideON(WindType wt);
extern	void      _Cdecl  WHideOFF(WindType wt);

/* 陳� Window Cursor 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WCursorON(void) ;
extern	void      _Cdecl  WCursorOFF(void) ;

extern	Coord     _Cdecl  WhereX(void) ;
extern	Coord     _Cdecl  WhereY(void) ;

extern	void      _Cdecl  WGotoXY(Coord X, Coord Y) ;

/* 陳� Window Line 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳 */

extern	void      _Cdecl  WInsLine(void) ;
extern	void      _Cdecl  WDelLine(void) ;
extern	void      _Cdecl  WClrEol(void) ;

/* 陳� Window Write 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

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

// メ�� l ┼� len <0, 皰 �┐�珮鉗┘ 0-�, ┃�腑 - �牀．��.

/* 陳� Window Get 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

extern	char      _Cdecl  WGetWindowChar(WindType wt, Coord X, Coord Y, Color *Fore, Color *Back) ;
extern	char      _Cdecl  WGetScreenChar(Coord X, Coord Y, Color *Fore, Color *Back) ;

/* 陳� Screen 陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳� */

extern	void      _Cdecl  WSaveScreen(ScrBuf *bf) ;
extern	void      _Cdecl  WRestoreScreen(ScrBuf *bf) ;

/* 様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様 */

#ifdef __cplusplus
}
#endif

#endif  /* __WINDOW_H */
