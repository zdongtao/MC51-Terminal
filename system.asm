AURX1   DATA	0A2H
XICON   DATA	0AEH
SFCF    DATA	0B1H


PUBLIC  _DO_ISP,_FAST_WRITE_ICON


;MEM_DT	SEGMENT	DATA
SYSTEM_PR	SEGMENT	CODE



RSEG	SYSTEM_PR


_FAST_WRITE_ICON:
        USING  0

        PUSH    AR3
        PUSH    AR4
        PUSH    AR5
        PUSH    AR6
        PUSH    AR7

	MOV  	DPL,R5
	MOV  	DPH,R4

	INC     AURX1               ; Set DPS to point to DPTR1

	MOV  	DPL,R7             ;DES ADDR
	MOV  	DPH,R6

	INC     AURX1             ; Set DPS to point to DPTR0
	LOOP: 
	MOVX 	A,@DPTR
	INC     DPTR              ; Increment source address                            2                                 
	INC     AURX1             ; DPTR1
	MOVX 	@DPTR,A
	INC     DPTR              ; Increment source address                            2                                 
        SWAP    A
	MOVX 	@DPTR,A
	INC     DPTR              ; Increment source address                            2                                 
	INC     AURX1             ; Set DPS to point to DPTR1
	DJNZ    R3, LOOP          ; Check if all done                 

        POP    AR7
        POP    AR6
        POP    AR5
        POP    AR4
        POP    AR3

	RET

_DO_ISP:  
        CLR   A
        MOV   IE, A         ;关闭所有中断使能  
        MOV   XICON, A 
        MOV   SFCF, A       ;地址映射，E000->0000
        LJMP  0000H         ;从ISP程序开始运行
        SJMP  $
        RET
/*
_LCD_OUTLINE:
        USING   0

        PUSH    AR5
        PUSH    AR6
        PUSH    AR7

	MOV  	DPL,R7
	MOV  	DPH,R6

        MOV     A,R5
        JNB     ACC.7, JMP_BIT6
        MOV     A, #07
	MOVX 	@DPTR, A 

JMP_BIT6:
        INC     DPTR
        JNB     ACC_6, JMP_BIT5
	MOVX 	@DPTR,#07  

JMP_BIT5:
        INC     DPTR
        JNB     ACC_5, JMP_BIT4
	MOVX 	@DPTR,#07  

JMP_BIT4:
        INC     DPTR
        JNB     ACC_4, JMP_BIT3
	MOVX 	@DPTR,#07  

JMP_BIT3:
        INC     DPTR
        JNB     ACC_3, JMP_BIT2
	MOVX 	@DPTR,#07  

JMP_BIT2:
        INC     DPTR
        JNB     ACC_2, JMP_BIT1
	MOVX 	@DPTR,#07  

JMP_BIT1:
        INC     DPTR
        JNB     ACC_1, JMP_BIT0
	MOVX 	@DPTR,#07  
           
JMP_BIT0:
        INC     DPTR
        JNB     ACC_0, JMP_RET
	MOVX 	@DPTR,#07  

JMP_RET:
        POP    AR7
        POP    AR6
        POP    AR5

	RET

*/
/*
_RD_MAC_BUF:
        USING  0

        PUSH    AR4
        PUSH    AR5
        PUSH    AR6
        PUSH    AR7

	MOV  	DPTR,#0C002H       ;MAC BUS ADDR AT DPTR1

	INC     AURX1             ;Set DPS to point to DPTR1

	MOV  	DPL,R7            ;DES ADDR 
	MOV  	DPH,R6

	INC     AURX1             ; Set DPS to point to DPTR0

LOOP_1: 
	MOVX 	A,@DPTR
	INC     AURX1             ; DPTR1
	MOVX 	@DPTR,A
	INC     DPTR              ; Increment source address                            2                                 
	INC     AURX1             ; Set DPS to point to DPTR1
	DJNZ    R5, LOOP_1          ; Check if all done                 
	DJNZ    R4, LOOP_1          ; Check if all done                 

        POP    AR7
        POP    AR6
        POP    AR5
        POP    AR4

	RET
*/


	END
