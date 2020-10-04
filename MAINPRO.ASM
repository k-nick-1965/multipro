;* �������������������������������������������������������������������� *;
;                           mainpro.asm                                  ;
;                                                                        ;
;                Sheduler`s multi-process  library                       ;
;                                                                        ;
;                       (C)  Kaling N.V. 1994                            ;
;                                                                        ;
;* �������������������������������������������������������������������� *;

        .model large,C
        LOCALS
        EXTRN   farfree
        EXTRN   FatalError

;--------------------------------------------------------

ProcessGuard     = 1802H
TimePeriod       = 55
HighPriority     = 101          ; ���� ���ਢ�����஢���� �ਮ���

;--------------------------------------------------------

FarAddr STRUC
 FA_Off         dw 0
 FA_Seg         dw 0
ENDS

SignalDef STRUC
 SI_Name        dw 0            ; ��� (�����) ᨣ����
 SI_Next        FarAddr <>      ; ᫥���騩 ᨣ��� � ᯨ᪥ ᨣ�����
 SI_NextActiv   FarAddr <>      ; ᫥���騩 ᨣ��� � ᯨ᪥ ��⨢��� ᨣ�����
 SI_NewTurn     FarAddr <>      ; 㪠��⥫� �� ��।� ����ᮢ
 SI_OldTurn     FarAddr <>      ; 㪠��⥫� �� ��।� ����ᮢ
 SI_Activ       dw 0            ; 䫠� ��⨢���� ᨣ����
ENDS

ProcessEnvironment STRUC
 PE_ProcAddr    FarAddr <>      ; ���� ��砫� ��楤��� �����
 PE_Next        FarAddr <>      ; ���� �������� ᫥���饣� �����
 PE_Pre         FarAddr <>      ; ���� �������� �।��饣� �����
 PE_NextWait    FarAddr <>      ; ᫥���騩 ����� � ��।� ᨣ����
 PE_PreWait     FarAddr <>      ; �।��騩 ����� � ��।� ᨣ����
 PE_NextIntr    FarAddr <>      ; ᫥���騩 ��ࢠ��� ᨣ����� �����
 PE_StackBegin  FarAddr <>      ; �㫥��� �஢��� �⥪�
 PE_Priority    dw 0
 PE_Lock        dw 0
 PE_Live        dw 1
 PE_Wait        dw 0
 PE_SP          dw 0
 PE_SS          dw 0
 PE_Guard       dw ProcessGuard
ENDS

;--------------------------------------------------------

                .data
BackProc                ProcessEnvironment <>
                        PUBLIC  BackProc
CurrProc                FarAddr <>
FirstSignal             FarAddr <>
                        PUBLIC  FirstSignal;
FirstActivSignal        FarAddr <>
                        PUBLIC  FirstActivSignal;
SchedulerWorking        dw      0
                        PUBLIC  SchedulerWorking;
SchedPeriod             dw      55
SchedCompensator        dw      0
TimeCompensator         dw      0
CurrentLock             dw      0
ActivMarks              dw      0
                        PUBLIC  ActivMarks
SystemSignals           dd      101 dup (0)
                        PUBLIC  SystemSignals
TimeMeter               dw      0
                        ;         1, 2, 4, 8, 16,32,64,128,256,512,1024,
MarkSignal              dw      0,30,32,34,36,38,40,42,44, 46, 48, 50
                        ;       2048,4096,8192,16384,32768
                        dw      52,  54,  56,  58,   60
                        PUBLIC  MarkSignal
InterruptTurn           FarAddr <>

;--------------------------------------------------------

                .code
Old08                   FarAddr <>
TextBadCurrProcess:     db      "Sch- Fatal Error : Destroed current process data area. ",0,0
TextBadNextProcess:     db      "Sch- Fatal Error : Destroed next process data area. ",0,0
TextNotSystemSignal:    db      "Sch- Fatal Error : Indefinition system signal. ",0,0

;--------------------------------------------------------

                .code

;--------------------------------------------------------

MultiTimer:     PUBLIC  MultiTimer
                cli
                push    ax
                push    bx
                push    dx
                push    cx
                push    ds
                mov     ax,@data
                mov     ds,ax

                mov     ax,TimeMeter
                mov     dx,ax
                inc     ax
                mov     TimeMeter,ax
                xor     dx,ax
                and     dx,ActivMarks           ; ��⠢�塞 ⮫쪮 ��⨢�� ��⪨
                xor     cx,cx                   ; 䫠� ����室����� ��९����஢����
                xor     bx,bx
NextMark:       inc     bx                      ; �����
                inc     bx                      ;    ��⪨
                or      dx,dx                   ; ���� ��⨢�� ��⪨ ?
                je      EmptyMarks              ; ���
                shr     dx,1
                jnc     NextMark
                inc     cx
                mov     ax,MarkSignal[bx]       ; �롨ࠥ� ����� ᨣ����, ᮮ⢥�����饣� ��⪥
                push    ax
                call    far ptr SystemSend      ; ���뫠�� ᨣ���
                pop     ax
                jmp     short NextMark

EmptyMarks:     inc     TimeCompensator
                cmp     TimeCompensator,TimePeriod
                jb      NotStdTimer
                sub     TimeCompensator,TimePeriod

StdTimer:       pushf
                call    dword ptr cs:OLD08                      ; ���� ��ࠡ��稪
                jmp     short SchedAct

NotStdTimer:    mov     al,20H
                out     20H,al

SchedAct:       mov     ax,SchedPeriod
                inc     SchedCompensator
                or      cx,cx
                jne     YesSched                        ; ��९����஢���� �� ��⪥
                cmp     SchedCompensator,ax
                jb      NotSched                        ; ��९����஢���� �� ������
                mov     SchedCompensator,0

YesSched:       pop     ds
                pop     cx
                pop     dx
                pop     bx
                pop     ax
                jmp     Scheduler

NotSched:       pop     ds
                pop     cx
                pop     dx
                pop     bx
                pop     ax
                iret

;--------------------------------------------------------

Scheduler:      PUBLIC  Scheduler
                cli
                push    ax
                push    bx
                push    cx
                push    dx
                push    di
                push    si
                push    bp
                push    ds
                push    es
                mov     ax,@data
                mov     ds,ax
                les     bx,CurrProc
                mov     es:[bx].PE_SS,ss
                mov     es:[bx].PE_SP,sp

TestGuard:      cmp     es:[bx].PE_Guard,ProcessGuard
                je      GoodGuard
                lea     ax,TextBadCurrProcess
                call    GrobMashina
GoodGuard:
                mov     ax,es:[bx].PE_Lock
                mov     CurrentLock,ax

SignExe:        les     bx,FirstActivSignal
DoubleSignExe:  mov     ax,es
                add     ax,bx                                   ; ���� ��⨢�� ᨣ���� ?
                jnz     @@1
                jmp     IntrTest                                ; ���
          @@1:  mov     ax,es:[bx].SI_OldTurn.FA_Off
                add     ax,es:[bx].SI_OldTurn.FA_Seg            ; ���� ����� � ��।� ?
                jnz     SignAct                                 ; ��
                mov     es:[bx].SI_Activ,0                      ; ���
                les     bx,dword ptr es:[bx].SI_NextActiv       ;
                mov     FirstActivSignal.FA_Off,bx              ; ���ન���� ᨣ���
                mov     FirstActivSignal.FA_Seg,es
                jmp     short SignExe

SignAct:        les     bx,dword ptr es:[bx].SI_OldTurn
                cmp     CurrentLock,0                           ; ࠧ�襭� ��९����஢���� ��ࢠ����� ����� ?
                je      IntrIns                                 ; ��
                cmp     es:[bx].PE_Priority,HighPriority        ; ���, ���� ����� ��।� ����� �ਢ�����஢���� �ਮ��� ?
                jb      ClearProc                               ; ��
                jmp     MultiBack                               ; ���

IntrIns:        mov     ax,InterruptTurn.FA_Seg                 ;�஢�ઠ ������ ��ࢠ���� ����ᮢ
                add     ax,InterruptTurn.FA_Off
                jz      ClearProc                               ; ���� ��ࢠ��� ������ ?
                les     bx,InterruptTurn                        ; �஢�ઠ ������ ��ࢠ���� ����ᮢ
                xor     ax,ax                                   ; ��
                xchg    ax,es:[bx].PE_NextIntr.FA_Seg
                mov     InterruptTurn.FA_Seg,ax
                xor     ax,ax
                xchg    ax,es:[bx].PE_NextIntr.FA_Off
                mov     InterruptTurn.FA_Off,ax
                jmp     Activate

ClearProc:      mov     ax,es:[bx].PE_NextWait.FA_Off           ; ���ન����
                mov     dx,es:[bx].PE_NextWait.FA_Seg           ;    �����
                lds     di,FirstActivSignal                     ;      ��
                mov     ds:[di].SI_OldTurn.FA_Off,ax            ;        ��ன
                mov     ds:[di].SI_OldTurn.FA_Seg,dx            ;          ��।�
                mov     ax,@data
                mov     ds,ax
                mov     es:[bx].PE_Wait,0
                cmp     es:[bx].PE_Live,0                       ; ����� ��⨢�� ?
                jne     RememberPre                             ; ��
                jmp     SignExe                                 ; ���

RememberPre:                                                    ; ����������� ��ࢠ����� �����
                cmp     CurrentLock,0                           ; ࠧ�襭� ��९����஢���� ��ࢠ����� ����� ?
                je      Activate                                ; ��
                push    bx
                push    es
                mov     ax,@data
                mov     ds,ax
                les     bx,CurrProc
                mov     ax,InterruptTurn.FA_Seg
                mov     es:[bx].PE_NextIntr.FA_Seg,ax
                mov     InterruptTurn.FA_Seg,es
                mov     ax,InterruptTurn.FA_Off
                mov     es:[bx].PE_NextIntr.FA_Off,ax
                mov     InterruptTurn.FA_Off,bx
                pop     es
                pop     bx
                jmp     short Activate

IntrTest:       les     bx,InterruptTurn                        ;�஢�ઠ ������ ��ࢠ���� ����ᮢ
                mov     ax,es
                add     ax,bx
                jz      MultiBack                               ; ���� ��ࢠ��� ������ ?
                xor     ax,ax                                   ; ��
                xchg    ax,es:[bx].PE_NextIntr.FA_Seg
                mov     InterruptTurn.FA_Seg,ax
                xor     ax,ax
                xchg    ax,es:[bx].PE_NextIntr.FA_Off
                mov     InterruptTurn.FA_Off,ax
                jmp     short Activate

MultiBack:      xor     cx,cx
                les     bx,CurrProc

LockTest:       cmp     CurrentLock,0                           ; ������ ࠧ�襭� ��९����஢���� ?
                je      GoodLock
                jmp     RestAndExit

GoodLock:       les     bx,dword ptr es:[bx].PE_Next

TestFree:       mov     ax,es
                add     ax,bx                                   ; ��諨 �� ���� ��।� ?
                jnz     Activate
                or      cx,cx                                   ; �஢�ઠ �� ��横�������, �.�.
                jne     BackExit                                ; ������⢨� ��⨢���
                inc     cx                                      ; ����� � ��।�
                les     bx,BackProc.PE_Next                     ; ��룠�� � ��砫� ��।�
                jmp     short TestFree

BackExit:       lea     ax,BackProc
                mov     CurrProc.FA_Off,ax
                mov     CurrProc.FA_Seg,ds
                les     bx,CurrProc
                jmp     short RestAndExit

NextProc:       les     bx,dword ptr es:[bx].PE_Next
                jmp     short TestFree

Activate:
TestNextGuard:  cmp     es:[bx].PE_Guard,ProcessGuard
                je      IsLive
                lea     ax,TextBadNextProcess
                call    GrobMashina
IsLive:         cmp     es:[bx].PE_Live,0                       ; ����� ��⨢�� ?
                je      NextProc
IsWait:         cmp     es:[bx].PE_Wait,0                       ; ����� �ਮ�⠭����� ?
                jne     NextProc
                mov     CurrProc.FA_Off,bx
                mov     CurrProc.FA_Seg,es

RestAndExit:    mov     SchedCompensator,0              ; ��⠭���� ᢥ���� ����� ��� �����
                mov     ss,es:[bx].PE_SS
                mov     sp,es:[bx].PE_SP
                pop     es
                pop     ds
                pop     bp
                pop     si
                pop     di
                pop     dx
                pop     cx
                pop     bx
                pop     ax
                iret

;--------------------------------------------------------

MainInitScheduler PROC  FAR     per:word
                PUBLIC  MainInitScheduler
                push    ax
                push    ds

                mov     ax,@data
                mov     ds,ax

                mov     ax,per
                mov     SchedPeriod,ax
                mov     SchedCompensator,0
                mov     TimeCompensator,0

                xor     ax,ax
                mov     [BackProc.PE_Next.FA_Off],ax
                mov     [BackProc.PE_Next.FA_Seg],ax
                mov     [SchedulerWorking],0

                pop     ds
                pop     ax
                ret
MainInitScheduler ENDP

;--------------------------------------------------------

_StartScheduler PROC    FAR
                PUBLIC  _StartScheduler

                cli
                push    ax
                push    bx
                push    ds
                push    es

                mov     ax,@data
                mov     ds,ax
                cmp     [SchedulerWorking],0
                jne     @@Exit
                mov     [SchedulerWorking],1

                lea     ax,BackProc                             ; ��⠭�������� �室��쭨�
                mov     CurrProc.FA_Off,ax                      ;    ⥪�騬 ����ᮬ
                mov     CurrProc.FA_Seg,ds

                xor     ax,ax
                mov     ds,ax
                les     ax,ds:[08*4]
                mov     cs:Old08.FA_Off,ax
                mov     cs:Old08.FA_Seg,es
                lea     ax,MultiTimer
                mov     ds:[08*4+0],ax
                mov     ds:[08*4+2],word ptr @code

                call    SetTimerNew

@@Exit:         pop     es
                pop     ds
                pop     bx
                pop     ax
                jmp     Scheduler
_StartScheduler ENDP

;--------------------------------------------------------

StopScheduler   PROC    FAR
                PUBLIC  StopScheduler
                pushf
                cli
                push    ax
                push    ds

                mov     ax,@data
                mov     ds,ax
                cmp     [SchedulerWorking],1
                jne     @@Exit
                mov     [SchedulerWorking],2

@@Rest08:       xor     ax,ax
                mov     ds,ax
                mov     ax,cs:Old08.FA_Off
                mov     ds:[08*4+0],ax
                mov     ax,cs:Old08.FA_Seg
                mov     ds:[08*4+2],ax

@@Exit:         call    SetTimerOld

                pop     ds
                pop     ax
                popf
                ret
StopScheduler   ENDP

;--------------------------------------------------------

ContinueScheduler       PROC    FAR
                PUBLIC  ContinueScheduler

                pushf
                cli
                push    ax
                push    bx
                push    ds
                push    es

                mov     ax,@data
                mov     ds,ax
                cmp     [SchedulerWorking],2
                jne     @@Exit
                mov     [SchedulerWorking],1

                xor     ax,ax
                mov     ds,ax
                les     ax,ds:[08*4]
                mov     cs:Old08.FA_Off,ax
                mov     cs:Old08.FA_Seg,es
                lea     ax,MultiTimer
                mov     ds:[08*4+0],ax
                mov     ds:[08*4+2],word ptr @code

                call    SetTimerNew

@@Exit:         pop     es
                pop     ds
                pop     bx
                pop     ax
                popf
                ret
ContinueScheduler       ENDP

;--------------------------------------------------------

_CallScheduler  PROC    FAR
                PUBLIC  _CallScheduler
                jmp     Scheduler
_CallScheduler  ENDP

;--------------------------------------------------------

_StopProcess    PROC    FAR
                PUBLIC  _StopProcess
                cli
                push    bx
                push    es
                mov     bx,@data
                mov     es,bx
                les     bx,es:CurrProc
                mov     es:[bx].PE_Live,0
                mov     es:[bx].PE_Lock,0
                pop     es
                pop     bx
                jmp     Scheduler
_StopProcess    ENDP

;--------------------------------------------------------

_KillProcess    PROC    FAR  nxt:dword
                PUBLIC  _KillProcess
                cli
                pop     ax
                pop     ax
                les     bx,nxt
                jmp     TestFree
_KillProcess    ENDP

;--------------------------------------------------------

CurrentProcess  PROC    FAR
                PUBLIC  CurrentProcess
                pushf
                cli
                push    es
                mov     ax,@data
                mov     es,ax
                mov     ax,es:CurrProc.FA_Off
                mov     dx,es:CurrProc.FA_Seg
                pop     es
                popf
                ret
CurrentProcess  ENDP

;--------------------------------------------------------

SetTimerNew     PROC    NEAR
                pushf
                cli
                push    ds

                mov     ax,@data                                ; ���樠������ ds
                mov     ds,ax

                mov     al,00110110B
                out     43H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2

                mov     ax,1188                                 ; 1 ms.
                out     40H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2
                mov     al,ah
                out     40H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2

                pop     ds
                popf
                ret
SetTimerNew     ENDP

;--------------------------------------------------------

SetTimerOld     PROC    NEAR
                pushf
                cli
                push    ax
                push    ds

                mov     ax,@data                                ; ���樠������ ds
                mov     ds,ax

                mov     al,00110110B
                out     43H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2
                mov     al,0FFH
                out     40H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2
                mov     al,0FFH
                out     40H,al
                jmp     $+2
                jmp     $+2
                jmp     $+2

                pop     ds
                pop     ax
                popf
                ret
SetTimerOld     ENDP

;--------------------------------------------------------

SystemSend      PROC    FAR   nam:word
                PUBLIC  SystemSend

                pushf
                cli
                push    ax
                push    bx
                push    di
                push    ds
                push    es

                mov     ax,@data
                mov     ds,ax

                mov     bx,nam
                sal     bx,1
                sal     bx,1
                les     bx,SystemSignals[bx]
                push    es
                push    bx
                mov     ax,es
                add     ax,bx
                jne     SignExist
                lea     ax,TextNotSystemSignal
                call    GrobMashina
SignExist:      cmp     es:[bx].SI_Activ,0              ; �஢�ઠ �� ������� ��⨢�����
                jne     SySiBadExit                     ; ����⪠ ����୮� ��⨢���樨
                mov     es:[bx].SI_Activ,1              ; 䫠� ��⨢����
                xor     ax,ax                           ; ��६�饭�� �����
                xchg    ax,es:[bx].SI_NewTurn.FA_Seg    ;  ��।�
                mov     es:[bx].SI_OldTurn.FA_Seg,ax    ;   � �����
                xor     ax,ax                           ;    � ���㫥���
                xchg    ax,es:[bx].SI_NewTurn.FA_Off    ;     �����
                mov     es:[bx].SI_OldTurn.FA_Off,ax    ;      ��।�
                lea     di,FirstActivSignal
                les     bx,FirstActivSignal
SetCicle:       mov     ax,es                           ; �஢�ઠ �� �㫥��� ����
                add     ax,bx
                je      SetSignal
                mov     ax,es:[bx].SI_Name
                cmp     ax,nam                          ; �஢�ઠ �� �ਮ���
                jae     SetSignal                       ; if N>=n
                mov     di,bx                           ; �������� ᫥���饣� ᨣ����
                add     di,SI_NextActiv
                push    es
                pop     ds
                les     bx, dword ptr es:[bx].SI_NextActiv
                jmp     short SetCicle
SetSignal:      pop     ds:[di].FA_Off
                pop     ds:[di].FA_Seg
                lds     di,ds:[di]
                mov     ds:[di].SI_NextActiv.FA_Seg,es
                mov     ds:[di].SI_NextActiv.FA_Off,bx

SySiExit:       pop     es
                pop     ds
                pop     di
                pop     bx
                pop     ax
                popf
                ret

SySiBadExit:    pop     ax
                pop     ax
                jmp     short SySiExit

SystemSend      ENDP
;--------------------------------------------------------

GrobMashina     PROC    NEAR
                push    cs
                push    ax
                call    StopScheduler
                call    far ptr FatalError
@@1:            jmp     short @@1
GrobMashina     ENDP

;--------------------------------------------------------


                END

