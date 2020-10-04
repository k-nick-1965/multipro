.MODEL LARGE
.CODE
ScrBuf               STRUC
   BufSize           DW ?
   Len               DW ?
   Heig              DW ?
   PointBuf          DW ?
ScrBuf               ENDS
BSIZE                EQU WORD PTR [bp+16]
		     PUBLIC  _CopyMainBufToScreen
                     EXTRN bf:ScrBuf
_CopyMainBufToScreen PROC FAR
                     mov ax,@data
                     mov ds,ax
		     push bp
		     mov bp,sp
		     push es
		     push ds
		     push si
		     push di
		     push cx
                     mov cx,SEG bf
		     push cx
                     mov si,OFFSET bf
		     pop ds
		     xor cx,cx
		     mov es,cx
                     mov di,es:[44Eh]
		     mov cx,0B800h
		     mov es,cx
                     mov ax,bsize
                     mov bx,2
                     div bx
		     mov cx,ax
		     cld
		rep  movsw
		     pop cx
		     pop di
		     pop si
		     pop ds
		     pop es
		     pop bp
		     ret
_CopyMainBufToScreen ENDP
		     END



