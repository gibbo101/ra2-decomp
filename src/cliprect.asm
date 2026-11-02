;******************************************************************************
;*                               O P E N  T S
;******************************************************************************
;* SPDX-License-Identifier: GPL-3.0-or-later
;* Copyright 2025 Electronic Arts Inc.
;* Copyright 2026 OpenTS contributors
;*
;* Contains material derived from Electronic Arts source code.
;* Modified by OpenTS contributors, 2026.
;* EA's GPLv3 Section 7 additional terms and supplemental warranty
;* disclaimers apply; see LICENSE.md.
;******************************************************************************

;***************************************************************************
;**   C O N F I D E N T I A L --- W E S T W O O D   A S S O C I A T E S   **
;***************************************************************************
;*                                                                         *
;*                 Project Name : Support Library                          *
;*                                                                         *
;*                    File Name : cliprect.asm                             *
;*                                                                         *
;*                   Programmer : Julio R Jerez                            *
;*                                                                         *
;*                   Start Date : Mar, 2 1995                              *
;*                                                                         *
;*                                                                         *
;*-------------------------------------------------------------------------*
;* Functions:                                                              *
;* int Clip_Rect ( int * x , int * y , int * dw , int * dh , 		   *
;*	       	   int width , int height ) ;          			   *
;* int Confine_Rect ( int * x , int * y , int * dw , int * dh , 	   *
;*	       	      int width , int height ) ;          		   *
;*									   *
;* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *


;IDEAL
;P386
;MODEL USE32 FLAT
.386
.model flat, C

;GLOBAL	 C Clip_Rect	:NEAR
;GLOBAL	 C Confine_Rect	:NEAR
PUBLIC Clip_Rect
PUBLIC Confine_Rect

;CODESEG
.CODE

;***************************************************************************
;* Clip_Rect -- clip a given rectangle against a given window		   *
;*                                                                         *
;* INPUT:   &x , &y , &w , &h  -> Pointer to rectangle being clipped       *
;*          width , height     -> dimension of clipping window             *
;*                                                                         *
;* OUTPUT: a) Zero if the rectangle is totally contained by the 	   *
;*	      clipping window.						   *
;*	   b) A negative value if the rectangle is totally outside the     *
;*            the clipping window					   *
;*	   c) A positive value if the rectangle	was clipped against the	   *
;*	      clipping window, also the values pointed by x, y, w, h will  *
;*	      be modified to new clipped values	 			   *
;*									   *
;*   05/03/1995 JRJ : added comment                                        *
;*=========================================================================*
; int Clip_Rect (int* x, int* y, int* dw, int* dh, int width, int height);          			   *

; FUNCTION: GAMEMD 0x007C5DB0
Clip_Rect PROC near \
	uses ebx ecx edx esi edi \
	, x:dword \
	, y:dword \
	, w:dword \
	, h:dword \
	, _width:dword \
	, height:dword


	;This Clipping algorithm is a derivation of the very well known
	;Cohen-Sutherland Line-Clipping test. Due to its simplicity and efficiency
	;it is probably the most commontly implemented algorithm both in software
	;and hardware for clipping lines, rectangles, and convex polygons against
	;a rectagular clipping window. For reference see
	;"COMPUTER GRAPHICS principles and practice by Foley, Vandam, Feiner, Hughes
	; pages 113 to 177".
	; Briefly consist in computing the Sutherland code for both end point of
	; the rectangle to find out if the rectangle is:
	; - trivially accepted (no further clipping test, return the oroginal data)
	; - trivially rejected (return with no action, return error code)
	; - retangle must be iteratively clipped again edges of the clipping window
	;   and return the clipped rectangle

	; get all four pointer into regisnters
	mov	esi,[x]		; esi = pointer to x
	mov	edi,[y]		; edi = pointer to x
	mov	eax,[w]		; eax = pointer to dw
	mov	ebx,[h]		; ebx = pointer to dh

	; load the actual data into reg
	mov	esi,[esi]	; esi = x0
	mov	edi,[edi]	; edi = y0
	mov	eax,[eax]	; eax = dw
	mov	ebx,[ebx]	; ebx = dh

	; create a wire frame of the type [x0,y0] , [x1,y1]
	add	eax,esi		; eax = x1 = x0 + dw
	add	ebx,edi		; ebx = y1 = y0 + dh

	; we start we suthenland code0 and code1 set to zero
	xor 	ecx,ecx		; cl = sutherland boolean code0
	xor 	edx,edx		; dl = sutherland boolean code0

	; now we start computing the to suthenland boolean code for x0 , x1
	shld	ecx,esi,1	; bit3 of code0 = sign bit of (x0 - 0)
	shld	edx,eax,1 	; bit3 of code1 = sign bit of (x1 - 0)
	sub	esi,[_width]	; get the difference (x0 - (width + 1))
	sub	eax,[_width]	; get the difference (x1 - (width + 1))
	dec	esi
	dec	eax
	shld	ecx,esi,1	; bit2 of code0 = sign bit of (x0 - (width + 1))
	shld	edx,eax,1	; bit2 of code1 = sign bit of (x0 - (width + 1))

	; now we start computing the to suthenland boolean code for y0 , y1
	shld	ecx,edi,1   	; bit1 of code0 = sign bit of (y0 - 0)
	shld	edx,ebx,1	; bit1 of code1 = sign bit of (y0 - 0)
	sub	edi,[height]	; get the difference (y0 - (height + 1))
	sub	ebx,[height]	; get the difference (y1 - (height + 1))
	dec	edi
	dec	ebx
	shld	ecx,edi,1	; bit0 of code0 = sign bit of (y0 - (height + 1))
	shld	edx,ebx,1	; bit0 of code1 = sign bit of (y1 - (height + 1))

	; Bit 2 and 0 of cl and bl are complemented
	xor	cl,5		; reverse bit2 and bit0 in code0
	xor	dl,5 		; reverse bit2 and bit0 in code1

	; now perform the rejection test
	mov	eax,-1		; set return code to false
	mov	bl,cl 		; save code0 for future use
	test	dl,cl  		; if any two pair of bit in code0 and code1 is set
	jnz	clip_out	; then rectangle is outside the window

	; now perform the aceptance test
	xor	eax,eax		; set return code to true
	or	bl,dl		; if all pair of bits in code0 and code1 are reset
	jz	clip_out	; then rectangle is insize the window.								      '

	; we need to clip the rectangle iteratively
	mov	eax,-1		; set return code to false
	test	cl,1000b	; if bit3 of code0 is set then the rectangle
	jz	left_ok	; spill out the left edge of the window
	mov	edi,[x]		; edi = a pointer to x0
	mov	ebx,[w]		; ebx = a pointer to dw
	mov	esi,[edi]	; esi = x0
	mov	dword ptr [edi],0 ; set x0 to 0 "this the left edge value"
	add	[ebx],esi	; adjust dw by x0, since x0 must be negative

left_ok:
	test	cl,0010b	; if bit1 of code0 is set then the rectangle
	jz	bottom_ok	; spill out the bottom edge of the window
	mov	edi,[y]		; edi = a pointer to y0
	mov	ebx,[h]		; ebx = a pointer to dh
	mov	esi,[edi]	; esi = y0
	mov	dword ptr [edi],0 ; set y0 to 0 "this the bottom edge value"
	add	[ebx],esi	; adjust dh by y0, since y0 must be negative

bottom_ok:
	test	dl,0100b	; if bit2 of code1 is set then the rectangle
	jz	right_ok	; spill out the right edge of the window
	mov	edi,[w] 	; edi = a pointer to dw
	mov	esi,[x]		; esi = a pointer to x
	mov	ebx,[_width]	; ebx = the width of the window
	sub	ebx,[esi] 	; the new dw is the difference (width-x0)
	mov	[edi],ebx	; adjust dw to (width - x0)
	jle	clip_out	; if (width-x0) = 0 then the clipped retangle
				; has no width we are done
right_ok:
	test	dl,0001b	; if bit0 of code1 is set then the rectangle
	jz	clip_ok	; spill out the top edge of the window
	mov	edi,[h] 	; edi = a pointer to dh
	mov	esi,[y]		; esi = a pointer to y0
	mov	ebx,[height]	; ebx = the height of the window
	sub	ebx,[esi] 	; the new dh is the difference (height-y0)
	mov	[edi],ebx	; adjust dh to (height-y0)
	jle	clip_out	; if (width-x0) = 0 then the clipped retangle
				; has no width we are done
clip_ok:
	mov	eax,1  	; signal the calling program that the rectangle was modify
clip_out:
	ret

Clip_Rect	ENDP


;***************************************************************************
;* Confine_Rect -- clip a given rectangle against a given window	   *
;*                                                                         *
;* INPUT:   &x,&y,w,h    -> Pointer to rectangle being clipped       *
;*          width,height     -> dimension of clipping window             *
;*                                                                         *
;* OUTPUT: a) Zero if the rectangle is totally contained by the 	   *
;*	      clipping window.						   *
;*	   c) A positive value if the rectangle	was shifted in position    *
;*	      to fix inside the clipping window, also the values pointed   *
;*	      by x, y, will adjusted to a new values	 		   *
;*									   *
;*  NOTE:  this function make not attempt to verify if the rectangle is	   *
;*	   bigger than the clipping window and at the same time wrap around*
;*	   it. If that is the case the result is meaningless		   *
;*=========================================================================*
; int Confine_Rect (int* x, int* y, int dw, int dh, int width, int height);          			   *

Confine_Rect PROC near \
	uses ebx esi edi \
	, x:dword \
	, y:dword \
	, w:dword \
	, h:dword \
	, _width:dword \
	, height:dword

	xor	eax,eax
	mov	ebx,[x]
	mov	edi,[w]

	mov	esi,[ebx]
	add	edi,[ebx]

	sub	edi,[_width]
	neg	esi
	dec	edi

	test	esi,edi
	jl	x_axix_ok
	mov	eax,1

	test	esi,esi
	jl	shift_right
	mov	dword ptr [ebx],0
	jmp	x_axix_ok
shift_right:
	inc	edi
	sub	[ebx],edi
x_axix_ok:
	mov	ebx,[y]
	mov	edi,[h]

	mov	esi,[ebx]
	add	edi,[ebx]

	sub	edi,[height]
	neg	esi
	dec	edi

	test	esi,edi
	jl	confi_out
	mov	eax,1

	test	esi,esi
	jl	shift_top
	mov	dword ptr [ebx],0

	ret
shift_top:
	inc	edi
	sub	[ebx],edi
confi_out:
	ret

Confine_Rect	ENDP

END
