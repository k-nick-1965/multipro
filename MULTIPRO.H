/* �������������������������������������������������������������������� *\
                             multipro.h

            Additional sheduler`s multi-process  library

                        (C)  Kaling N.V. 1994

\* �������������������������������������������������������������������� */

#if     !defined(__MULTIPRO_H)
#define __MULTIPRO_H


/* �������������������������������������������������������������������� */

#ifdef __cplusplus
extern "C" {
#endif

/* �������������������������������������������������������������������� */

//#include "MainPro.h"

#if     !defined(boolean)
typedef int boolean;
#define TRUE  ((boolean) 1)
#define FALSE ((boolean) 0)
#define loop while (0==0)
#endif

/* �������������������������������������������������������������������� */


struct ProcEnvir {                                   // in mainpro.asm
                void                 (*ProcAddr)();  // ���� ��楤��� �����
                struct ProcEnvir*    Next;           // ���� ������ ᫥� �����
                struct ProcEnvir*    Pre;            // ���� �।��饣� �����
                struct ProcEnvir*    NextWait;       // ᫥���騩 ����� � ��।� ᨣ����
                struct ProcEnvir*    PreWait;        // �।��騩 ����� � ��।� ᨣ����
		struct ProcEnvir*    NextIntr;       // ᫥���騩 ��ࢠ��� ᨣ����� �����
		unsigned int*        StackBegin;     // ��砫� �⥪�
                unsigned int         Priority;       // �ਮ��� �����
                int                  Lock;           // 䫠� ࠧ�襭�� ��९����஢����
                int                  Live;           // 䫠� ����⢮����� �����
                int                  Wait;           // 䫠� �������� ����ᮬ ᨣ����
                int                  SP;             // 㪠��⥫� �⥪�
                int                  SS;             // ᥣ���� �⥪�
                int                  Guard;          // ����⠭� ����஫�
              };

typedef struct ProcEnvir *ProcType;                  // in mainpro.asm

struct SignalRec {                                   // in mainpro.asm
                unsigned int        Name;            // ��� (�����,�ਮ���) ᨣ����
                struct SignalRec*   Next;            // ᫥���騩 ���樠����஢���� ᨣ���
                struct SignalRec*   NextActiv;       // ᫥���騩 ��⨢���஢����   ᨣ���
                ProcType            NewTurn;         // ����� ��।�
                ProcType            OldTurn;         // ���� ��।�
                int                 Activ;           // 䫠� ��⨢����
               };

typedef struct SignalRec  SignalDef;                    // in mainpro.asm
typedef struct SignalRec *SignalType;                   // in mainpro.asm

/* �������������������������������������������������������������������� */

extern  struct ProcEnvir        BackProc;               // in mainpro.asm
extern  void                   *CurrProc;               // in mainpro.asm
extern  SignalType              FirstSignal;            // in mainpro.asm
extern  SignalType              FirstActivSignal;       // in mainpro.asm
extern  SignalType              SystemSignals[101];     // in mainpro.asm
extern  unsigned int            ActivMarks;             // in mainpro.asm
extern  unsigned int            MarkSignal[17];         // in mainpro.asm

/* �������������������������������������������������������������������� */

/*extern void                     _StartScheduler();      // in mainpro.asm
extern void                     _StopProcess();         // in mainpro.asm
extern void                     _CallScheduler();       // in mainpro.asm
*/
#define StartScheduler()        asm pushf;\
                                _StartScheduler();
                                                        // in mainpro.asm
#define StopProcess()           asm pushf;\
                                _StopProcess();
                                                        // in mainpro.asm
#define ProcessTransfer()       asm pushf;\
                                asm pushf;\
                                asm cli;\
                                _CallScheduler();\
                                asm popf;
                                                        // in mainpro.asm

/* �������������������������������������������������������������������� */

extern  void          InitScheduler(unsigned int PeriodMS) ;

extern  ProcType      InitProcess( void (*Proc)(),int ArraySize, unsigned int Priority);
extern  void          StartProcess    (ProcType PE) ;
extern  void          ContinueProcess (ProcType PE) ;
extern  void          LockProcess     (ProcType PE) ;
extern  void          UnLockProcess   (ProcType PE) ;
extern  ProcType      CurrentProcess    ();              // in mainpro.asm
extern  void          KillProcess       ();
extern  void          StopScheduler     ();              // in mainpro.asm
extern  void          ContinueScheduler ();              // in mainpro.asm
extern  void          LockCurrentProcess   () ;
extern  void          UnLockCurrentProcess () ;

extern  void          _StartScheduler();                 // in mainpro.asm
extern  void          _CallScheduler();                  // in mainpro.asm
extern  void          _StopProcess();                    // in mainpro.asm
extern  void          _KillProcess(ProcType Next);       // in mainpro.asm

extern  void          InitSignal (unsigned int Name) ;
extern  void          WaitSignal (unsigned int Name) ;
extern  void          SendSignal (unsigned int Name) ;

extern  unsigned int  SetMark (unsigned int Period) ;
extern  unsigned int  ClrMark (unsigned int Period) ;
extern  boolean       IsMark  (unsigned int Period) ;

/* �������������������������������������������������������������������� */

extern  struct ProcEnvir        BackProc;               // in mainpro.asm
extern  void                   *CurrProc;               // in mainpro.asm
extern  SignalType              FirstSignal;            // in mainpro.asm
extern  SignalType              FirstActivSignal;       // in mainpro.asm
extern  SignalType              SystemSignals[101];     // in mainpro.asm
extern  unsigned int            ActivMarks;             // in mainpro.asm
extern  unsigned int            MarkSignal[17];         // in mainpro.asm


/* �������������������������������������������������������������������� */

#ifdef __cplusplus
}
#endif

#endif  /* __MULTIPRO_H */



