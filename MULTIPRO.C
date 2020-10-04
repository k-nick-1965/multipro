/* ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ *\
                             multipro.c

            Additional sheduler`s multi-process  library

                        (C)  Kaling N.V. 1994

\* ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
#include <stdlib.h>
#include <dos.h>
#include <alloc.h>

#include "MultiPro.h"
#include "MainPro.h"
#include "Window.h"

extern   void InitMultiPro();
extern   void AbortMultiPro();

#pragma  startup InitMultiPro   76
#pragma  exit    AbortMultiPro  76

ProcType InitProcess( void (*Proc)(),int ArraySize, unsigned int Priority){
ProcType PE,pt;
unsigned int i,*sp;
  LockCurrentProcess();
  asm pushf;
  asm cli;
  if (ArraySize<2048) ArraySize=2048;
  PE = (ProcType) farmalloc(ArraySize);
  asm cli;
  if (PE!=NULL) {
    PE->ProcAddr = Proc;
    PE->Lock       = FALSE;
    PE->Live       = FALSE;
    PE->Wait       = FALSE;
    PE->NextWait   = NULL;
    PE->PreWait    = NULL;
    PE->Priority   = Priority;
    PE->StackBegin = MK_FP(FP_SEG(PE),(FP_OFF(PE)+ArraySize-1));
    sp=PE->StackBegin;
    *(sp--)=0x202;
    *(sp--)=FP_SEG(Proc);
    *(sp--)=FP_OFF(Proc);
    for (i=1; i<=7; i++) *(sp--)=0;
    *(sp--)=_DS;
    *(sp)  =_ES;
    PE->SS=FP_SEG(sp);
    PE->SP=FP_OFF(sp);
    PE->Guard      = 0x1802; //ProcessGuard;

    pt=&BackProc;
    while (pt->Next!=NULL) {
      pt=pt->Next;
    }
    PE->Next = NULL;
    pt->Next = PE;
    PE->Pre = pt;
  }
  asm popf;
  UnLockCurrentProcess();
  return PE;
}

void StartProcess(ProcType PE) {
unsigned int i,*sp;
  if (PE!=NULL) {
    asm pushf
    asm cli;
    PE->Lock = FALSE;
    PE->Live = TRUE;
    PE->Wait = FALSE;
    sp=PE->StackBegin;
    *(sp--)=0x202;
    *(sp--)=FP_SEG(PE->ProcAddr);
    *(sp--)=FP_OFF(PE->ProcAddr);
    for (i=1; i<=7; i++) *(sp--)=0;
    *(sp--)=_DS;
    *(sp)  =_ES;
    PE->SS=FP_SEG(sp);
    PE->SP=FP_OFF(sp);
    asm popf;
    ProcessTransfer();
  }
}

void ContinueProcess(ProcType PE) {
  asm pushf
  asm cli
  PE->Live = TRUE;
  asm popf
}

void KillProcess() {
ProcType PE,NXT;
  LockCurrentProcess();
  asm cli
  PE = CurrentProcess();
  NXT=PE->Next;
  PE->Next->Pre=PE->Pre;
  PE->Pre->Next=PE->Next;
  if (PE->Wait!=0) {
    asm nop;
  }
  farfree(PE);
  asm cli;
  if (PE->Guard == 0x1802) PE->Guard = 0x0013;
  _KillProcess(NXT);
}


void LockProcess(ProcType PE) {
  asm pushf
  asm cli
  PE->Lock++;
  asm popf
}

void UnLockProcess(ProcType PE) {
  asm pushf
  asm cli
  if (PE->Lock) PE->Lock--;
  asm popf
}

void LockCurrentProcess() {
ProcType PE;
  asm pushf
  asm cli
  PE = CurrentProcess();
  PE->Lock++;
  asm popf
}

void UnLockCurrentProcess() {
ProcType PE;
  asm pushf
  asm cli
  PE = CurrentProcess();
  if (PE->Lock) PE->Lock--;
  asm popf
}

void InitScheduler(unsigned int PeriodMS) {
  SetWindowMultiProcess(LockCurrentProcess,UnLockCurrentProcess,(long)CurrentProcess);
  MainInitScheduler(PeriodMS);
}

void InitSignal(unsigned int Name) {
SignalType st;
  LockCurrentProcess();
  asm pushf;
  asm cli;
  st=FirstSignal;
  if (Name>100) {
    while (st!=NULL) {
      if (st->Name==Name) { FatalError(" Fatal Error: Duplicate signal initialization. "); }
      st = st->Next;
    }
  }
  st=farmalloc(sizeof(SignalDef));
  asm cli;
  if (st==NULL) { FatalError(" Fatal Error: Too small memory for signal. "); }
  st->Name=Name;
  st->Next=FirstSignal;
  st->NextActiv=NULL;
  st->NewTurn=NULL;
  st->OldTurn=NULL;
  st->Activ=0;
  FirstSignal=st;
  if (Name<=100) { SystemSignals[Name]=st; };
  asm popf;
  UnLockCurrentProcess();
}

void WaitSignal(unsigned int Name) {
SignalType  st;
ProcType    pr,Cpr,pre,*pread;
  Cpr=CurrentProcess();
  if (Cpr->Wait) {return;}
  LockCurrentProcess();
  asm pushf;
  asm cli;
  if (Name<=100) {
    st=SystemSignals[Name];
    if (st==NULL) { FatalError(" Fatal Error: Indefinition system signal. "); }
  }
  else {
//    asm pushf;
//    asm cli;
    st=FirstSignal;
    while (st->Name!=Name) {
      if (st==NULL) { FatalError(" Fatal Error: Indefinition signal. "); }
      st = st->Next;
    }
//    asm popf;
  }
  Cpr->Wait=TRUE;
  pr=st->NewTurn;
  pre=NULL;
  pread=&st->NewTurn;
  while ((Cpr->Priority>=pr->Priority) && (pr!=NULL)) {
    pread=&pr->NextWait;
    pre=pr;
    pr=pr->NextWait;
  }
  Cpr->NextWait=pr;
  Cpr->PreWait=pre;
  *pread=Cpr;
  asm popf;
  UnLockCurrentProcess();
  Cpr->Lock=0;
  ProcessTransfer();
}

void SendSignal(unsigned int Name) {
SignalType  ast,st,*pread;
  LockCurrentProcess();
  asm pushf;
  asm cli;
  if (Name<=100) {
    st=SystemSignals[Name];
    if (st==NULL) { FatalError(" Fatal Error: Indefinition system signal. "); }
  }
  else {
    st=FirstSignal;
    while (st->Name!=Name) {
      if (st==NULL) { FatalError(" Fatal Error: Indefinition signal. "); }
      st = st->Next;
    }
  }
  if (st->Activ==1) {
    asm popf;
    UnLockCurrentProcess();
    return;                      // ฏฎฏ๋โช  ฏฎขโฎเญฎฉ  ชโจขจง ๆจจ
  }
  st->Activ=1;
  st->OldTurn=st->NewTurn;
  st->NewTurn=NULL;
  ast=FirstActivSignal;
  pread=&FirstActivSignal;
  if (ast!=NULL){
    while ((Name>=ast->Name) && (ast->NextActiv!=NULL)) {
      pread = &ast->NextActiv;
      ast   = ast->NextActiv;
    }
  }
  *pread=st;
  st->NextActiv=ast;
  asm popf;
  UnLockCurrentProcess();
  ProcessTransfer();
}

unsigned int SetMark(unsigned int Period) {
unsigned int i=1,j=1;
  while (i<=Period) { i=i<<1; j++; };
  if (Period<(i-(i>>2))) { i=i>>1; j--; };
  ActivMarks=ActivMarks | i;
  InitSignal(MarkSignal[j]);
  return i;
}

unsigned int ClrMark(unsigned int Period) {
unsigned int i=1;
  while (i<=Period) { i=i<<1; };
  if (Period<(i-(i>>2))) { i=i>>1; };
  ActivMarks=ActivMarks & (!i);
  return i;
}

boolean IsMark(unsigned int Period) {
unsigned int i=1;
  while (i<=Period) { i=i<<1; };
  if (Period<(i-(i>>2))) { i=i>>1; };
  if (ActivMarks & i) return TRUE;
  else return FALSE;
}

void InitMultiPro() {
  SetWindowMultiProcess(LockCurrentProcess,UnLockCurrentProcess,(long)CurrentProcess);
  MainInitScheduler(55);
  ActivMarks=0;
}

void AbortMultiPro() { StopScheduler(); }
