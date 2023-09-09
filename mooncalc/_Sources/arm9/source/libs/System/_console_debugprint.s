; /*---------------------------------------------------------------------------------
; 
; 	Copyright (C) 2008 - 2010
; 		Mukunda Johnson (eKid)
; 		Jason Rogers (dovoto)
; 		Dave Murphy (WinterMute)
; 
; 	This software is provided 'as-is', without any express or implied
; 	warranty.  In no event will the authors be held liable for any
; 	damages arising from the use of this software.
; 
; 	Permission is granted to anyone to use this software for any
; 	purpose, including commercial applications, and to alter it and
; 	redistribute it freely, subject to the following restrictions:
; 
; 	1.	The origin of this software must not be misrepresented; you
; 		must not claim that you wrote the original software. If you use
; 		this software in a product, an acknowledgment in the product
; 		documentation would be appreciated but is not required.
; 
; 	2.	Altered source versions must be plainly marked as such, and
; 		must not be misrepresented as being the original software.
; 
; 	3.	This notice may not be removed or altered from any source
; 		distribution.
; 
; ---------------------------------------------------------------------------------*/

;---------------------------------------------------------------------------------
        AREA     globals,CODE,READONLY
        THUMB
        
        EXPORT nocashMessage
;---------------------------------------------------------------------------------
; no$gba debug output function
;---------------------------------------------------------------------------------
nocashMessage  ; params = { string }
;---------------------------------------------------------------------------------
 
; max string length == 120 bytes
 
;---------------------------------------------------------------------------------
; copy string into buffer
;---------------------------------------------------------------------------------
        ldr     r1,=buffer  ; get buffer address
        movs    r2,#0       ; r2 = read/write position
loop
        ldrb    r3, [r0,r2] ; load character
        strb    r3, [r1,r2] ; store character
        cmps    r3, #0      ; character == NULL?
        beq     send           ; yes, send message
        adds    r2,#1      ; increment read/write position
        cmps    r2, #120    ; max length == 120
        bne     loop           ; loop if < 120 characters
 
;---------------------------------------------------------------------------------
; send message to no$
;---------------------------------------------------------------------------------
send
        mov     r12,r12     ; first ID
        b      exit           ; skip text
        DCW  0x6464      ; second ID
        DCW  0           ; flags
buffer  SPACE 120         ; data

exit
        bx lr
        
;---------------------------------------------------------------------------------
        END
