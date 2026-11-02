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

	;IDEAL
	.386P;P386
	.MODEL FLAT, C ;MODEL	USE32 FLAT
	;LOCALS	??
	.CODE;CODESEG


CODE_2BIT	EQU	0
CODE_4BIT	EQU	1
CODE_RAW	EQU	2
CODE_SILENCE	EQU	3
MAGICNUMBER	EQU	00000DEAFh
MAGICNUMBER2	EQU	0BABEBABEh


; avoids symbol conflicts with regular Unzap
_2bitdecode EQU _VQA_2bitdecode
_4bitdecode EQU _VQA_4bitdecode

;****************************************************************************
;*
;* NAME
;*     AudioUnzap - Uncompress zapped audio sample.
;*
;* SYNOPSIS
;*     Size = AudioUnzap(Source, Dest, Size)
;*
;*     long AudioUnzap(void *, void *, long);
;*
;* FUNCTION
;*     Decompress the zapped audio sample data into a buffer.
;*
;* INPUTS
;*     Source - Pointer to encoded audio data.
;*     Dest   - Pointer to buffer to decompress into.
;*     Size   - Maximum size of dest buffer.
;*
;* RESULT
;*     Size - Number of uncompressed bytes.
;*
;****************************************************************************


	PUBLIC	AudioUnzap
; FUNCTION: GAMEMD 0x007C7F20
	AudioUnzap PROC NEAR USES ebx ecx edx esi edi \
	\
	source:DWORD, \
	dest:DWORD, \
	count:DWORD

	LOCAL	previous:BYTE
	LOCAL	incount:DWORD

	pushfd
	cld
	mov	[incount],0	;Bytes read from source


;	Source, Dest and count must be valid.

	cmp	[source],0
	je	??fini

	cmp	[dest],0
	je	??fini

	cmp	[count],0
	je	??fini

	mov	esi,[source]	;Pointer to source data.
	mov	edi,[dest]	;Pointer to destination data.
	mov	ecx,[count]	;Number of bytes to fill dest buffer.
	mov	dl,080h	;Previous sample (starting value).

??mainloop:
	cmp	ecx,0	;If dest full then exit
	jle	??fini

	xor	eax,eax
	mov	al,[esi]	;Get code byte
	inc	[incount]
	inc	esi
	shl	eax,2	;AH contains code.
	shr	al,2	;AL contains sub-code data.

	cmp	ah,CODE_RAW	;Raw sequence?
	jne	short ??try4bit

;	The code contains either a 5 bit delta or a count of
;	raw samples to dump out.

	test	al,00100000b
	je	short ??justraw

;	The lower 5 bits are actually a signed delta.
;	Sign extend the delta and add it to the stream.

	shl	al,3
	sar	al,3
	add	dl,al
	mov	[edi],dl
	dec	ecx
	inc	edi
	jmp	??mainloop

;	The lower 5 bits hold a count of the number of raw
;	samples that follow this code. Dump these samples to
;	the output buffer.

??justraw:
	mov	ebx,ecx
	xor	ah,ah
	inc	al
	mov	ecx,eax
	shr	ecx,1
	rep	movsw
	adc	ecx,ecx
	rep	movsb
	mov	ecx,ebx
	add	[incount],eax
	sub	ecx,eax
	dec	edi
	mov	dl,[edi]	;Set "previous" value.
	inc	edi
	jmp	??mainloop

;	Check to see if this is a 4 bit delta code sequence.

??try4bit:
	inc	al	;Following codes use AL+1
	cmp	ah,CODE_4BIT
	jne	short ??try2bit

;	A sequence of 4bit deltas follow. AL equals the
;	number of nibble packed delta bytes to process.

??bit4loop:
	mov	ah,[esi]	;Fetch nibble packed delta codes
	mov	bl,ah
	inc	[incount]
	inc	esi

;	Add first delta to 'previous' sample already in DL.

	and	ebx,00001111b
	add	dl,[_4bitdecode+ebx]
	pushfd
	cmp	[_4bitdecode+ebx],0
	jl	short ??neg1

	popfd
	jnc	short ??ok1
	mov	dl,0FFh
	jmp	short ??ok1

??neg1:
	popfd
	jc	short ??ok1

	xor	dl,dl

??ok1:
	mov	dh,dl	;DH now holds new 'previous' sample.
	mov	bl,ah
	shr	bl,4
	add	dh,[_4bitdecode+ebx]
	pushfd
	cmp	[_4bitdecode+ebx],0
	jl	short ??neg2

	popfd
	jnc	short ??ok2

	mov	dh,0FFh
	jmp	short ??ok2

??neg2:
	popfd
	jc	short ??ok2

	xor	dh,dh

??ok2:
	mov	[edi],dx	;Output the two sample bytes
	sub	ecx,2
	add	edi,2

;	Put the correct 'previous' sample in DL where it belongs.

	mov	dl,dh

;	If there are more deltas to process then loop back.

	dec	al
	jnz	short ??bit4loop
	jmp	??mainloop

;	Check to see if 2 bit deltas need to be processed.

??try2bit:
	cmp	ah,CODE_2BIT
	jne	??zerodelta

;	A sequence of 2bit deltas follow.  AL equals the number of
;	packed delta bytes to process.

??bit2loop:
	mov	ah,[esi]	;Fetch packed delat codes
	inc	[incount]
	inc	esi

;	Add first delta to 'previous' sample already in DL.

	mov	bl,ah
	and	ebx,000011b
	add	dl,[_2bitdecode+ebx]
	pushfd
	cmp	[_2bitdecode+ebx],0
	jl	short ??neg3

	popfd
	jnc	short ??ok3

	mov	dl,0FFh
	jmp	short ??ok3

??neg3:
	popfd
	jc	short ??ok3
	xor	dl,dl

??ok3:
	mov	dh,dl
	ror	edx,8
	mov	bl,ah
	shr	ebx,2
	and	bl,00000011b
	add	dl,[_2bitdecode+ebx]
	pushfd
	cmp	[_2bitdecode+ebx],0
	jl	short ??neg4

	popfd
	jnc	short ??ok4

	mov	dl,0FFh
	jmp	short ??ok4

??neg4:
	popfd
	jc	short ??ok4

	xor	dl,dl

??ok4:
	mov	dh,dl
	ror	edx,8
	mov	bl,ah
	shr	ebx,4
	and	bl,00000011b
	add	dl,[_2bitdecode+ebx]
	pushfd
	cmp	[_2bitdecode+ebx],0
	jl	short ??neg5

	popfd
	jnc	short ??ok5

	mov	dl,0FFh
	jmp	short ??ok5

??neg5:
	popfd
	jc	short ??ok5

	xor	dl,dl

??ok5:
	mov	dh,dl
	ror	edx,8
	mov	bl,ah
	shr	ebx,6
	and	bl,00000011b
	add	dl,[_2bitdecode+ebx]
	pushfd
	cmp	[_2bitdecode+ebx],0
	jl	short ??neg6

	popfd
	jnc	short ??ok6

	mov	dl,0FFh
	jmp	short ??ok6

??neg6:
	popfd
	jc	short ??ok6

	xor	dl,dl

??ok6:
	ror	edx,8
	mov	[edi],edx	;Output two sample bytes
	sub	ecx,4
	add	edi,4

;	Put the correct 'previous' sample in DL where it belongs.

	rol	edx,8

;	If there are more deltas to process then loop back.

	dec	al
	jnz	??bit2loop
	jmp	??mainloop

;	There is a run of zero deltas.  Zero deltas merely duplicate
;	the 'previous' sample the requested number of times.

??zerodelta:
	xor	ebx,ebx
	mov	bl,al
	mov	al,dl
	sub	ecx,ebx
	xchg	ecx,ebx
	rep	stosb
	mov	ecx,ebx
	jmp	??mainloop

??fini:
	popfd
	mov	eax,[incount]
	ret

	AudioUnzap	ENDP

_2bitdecode	DB	-2,-1,0,1
_4bitdecode	DB	-9,-8,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,8

; ----------------------------------------------------------------
;
; Here are prototypes for the routines defined within this module:
;
; ULONG LCW_Uncompress(BYTE *source, BYTE *dest, ULONG length);
;
; ----------------------------------------------------------------

; FUNCTION: GAMEMD 0x007C8110
OLD_VQA_LCW_Uncompress proc C USES ebx ecx edx esi edi source:DWORD, dest:DWORD, _length:DWORD

	;USES ebx,ecx,edx,edi,esi

	;ARG	source:DWORD
	;ARG	dest:DWORD
	;ARG	length:DWORD
;LOCALS
	LOCAL a1stdest:DWORD
	LOCAL maxlen:DWORD
	LOCAL lastbyte:DWORD
	LOCAL lastcom:DWORD
	LOCAL lastcom1:DWORD


	mov	esi,[source]
	mov	edi,[dest]
	mov	edx,[_length]

;
;
; uncompress data to the following codes in the format b = byte, w = word
; n = byte code pulled from compressed data
;   Bit field of n		command		description
; n=0xxxyyyy,yyyyyyyy		short run	back y bytes and run x+3
; n=10xxxxxx,n1,n2,...,nx+1	med length	copy the next x+1 bytes
; n=11xxxxxx,w1			med run		run x+3 bytes from offset w1
; n=11111111,w1,w2		long copy	copy w1 bytes from offset w2
; n=11111110,w1,b1		long run	run byte b1 for w1 bytes
; n=10000000			end		end of data reached
;

	mov	[a1stdest],edi
	add	edx,edi
	mov	[lastbyte],edx
	cld			; make sure all lod and sto are forward
	mov	ebx,esi		; save the source offset

??loop:
	mov	eax,[lastbyte]
	sub	eax,edi		; get the remaining byte to uncomp
	jz	short ??out		; were done

	mov	[maxlen],eax	; save for string commands
	mov	esi,ebx		; mov in the source index

	lodsb
	or	al,al		; see if its a short run
	js	short ??notshort

	mov	ah,al		; put rel offset high nibble in ah
	and	ah,0Fh		; only 4 bits count
	sub	ecx,ecx
	mov	ch,al		;put count nibble in ch
	shr	ch,4
	mov	cl,ch
	xor	ch,ch
	add	ecx,3		; get actual run length

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??rsok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rsok:
	lodsb			; get rel offset low byte
	mov	ebx,esi		; save the source offset
	mov	esi,edi		; get the current dest
	sub	esi,eax		; get relative offset

	rep	movsb

	jmp	??loop

??notshort:
	test	al,40h		; is it a length?
	jne	short ??notlength	; if not it could be med or long run

	cmp	al,80h		; is it the end?
	je	short ??out		; if so its over

	mov	cl,al		; put the byte in count register
	and	ecx,3Fh		; and off the extra bits

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??lenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??lenok:
	rep movsb

	mov	ebx,esi		; save the source offset
	jmp	??loop

??out:
      	mov	eax,edi
	sub	eax,[a1stdest]
	jmp	short ??exit

??notlength:
	mov	cl,al		; get the entire code
	and	ecx,3Fh		; and off all but the size -3
	add	ecx,3		; add 3 for byte count

	cmp	al,0FEh
	jne	short ??notrunlength

	sub	eax,eax
	lodsw
	mov	ecx,eax
	sub	eax,eax
	lodsb
	mov	ebx,esi		;save the source offset

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??runlenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??runlenok:
	rep	stosb
	jmp	??loop

??notrunlength:
	cmp	al,0FFh		; is it a long run?
	jne	short ??notlong	; if not use the code as the size

	sub	eax,eax
	lodsw		; if so, get the size
	mov	ecx,eax

??notlong:
	lodsw			;get the real index
	mov	ebx,esi		;save the source offset
	add	eax,[a1stdest]	;add in the 1st index
	mov	esi,eax		;use eax as new source
	cmp	ecx,[maxlen]	;compare for overrun
	jbe	short ??runok	; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??runok:
	rep	movsb
	jmp	??loop

??exit:
	mov	eax,edi
	mov	ebx,[dest]
	sub	eax,ebx

	ret

OLD_VQA_LCW_Uncompress endp

VQA_LCW_Uncompress proc C USES ebx ecx edx edi esi source:DWORD, dest:DWORD, _length:DWORD

	;USES ebx,ecx,edx,edi,esi

	;ARG	source:DWORD
	;ARG	dest:DWORD
	;ARG	length:DWORD
;LOCALS
	LOCAL a1stdest:DWORD
	LOCAL maxlen:DWORD
	LOCAL lastbyte:DWORD
	LOCAL lastcom:DWORD
	LOCAL lastcom1:DWORD


	mov	edi,[dest]
	mov	esi,[source]
	mov	edx,[_length]

	mov	al,[esi]
	or	al,al
	jz ??rel

;
;
; uncompress data to the following codes in the format b = byte, w = word
; n = byte code pulled from compressed data
;   Bit field of n		command		description
; n=0xxxyyyy,yyyyyyyy		short run	back y bytes and run x+3
; n=10xxxxxx,n1,n2,...,nx+1	med length	copy the next x+1 bytes
; n=11xxxxxx,w1			med run		run x+3 bytes from offset w1
; n=11111111,w1,w2		long copy	copy w1 bytes from offset w2
; n=11111110,w1,b1		long run	run byte b1 for w1 bytes
; n=10000000			end		end of data reached
;

	mov	[a1stdest],edi
	add	edx,edi
	mov	[lastbyte],edx
	cld			; make sure all lod and sto are forward
	mov	ebx,esi		; save the source offset

??loop:
	mov	eax,[lastbyte]
	sub	eax,edi		; get the remaining byte to uncomp
	jz	short ??out		; were done

	js	short ??noop

	mov	[maxlen],eax	; save for string commands
	mov	esi,ebx		; mov in the source index
	sub	eax,eax
	lodsb
	or	al,al		; see if its a short run
	js	short ??notshort

	mov	ah,al		; put rel offset high nibble in ah
	and	ah,0Fh		; only 4 bits count
	sub	ecx,ecx
	mov	cl,al		;put count nibble in ch
	shr	cl,4		; get run -3
	add	ecx,3		; get actual run length

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??rsok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rsok:
	lodsb			; get rel offset low byte
	mov	ebx,esi		; save the source offset
	mov	esi,edi		; get the current dest
	sub	esi,eax		; get relative offset

	rep	movsb

	jmp	??loop

??notshort:
	test	al,40h		; is it a length?
	jne	short ??notlength	; if not it could be med or long run

	cmp	al,80h		; is it the end?
	je	short ??out		; if so its over

	mov	cl,al		; put the byte in count register
	and	ecx,3Fh		; and off the extra bits

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??lenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??lenok:
	rep movsb

	mov	ebx,esi		; save the source offset
	jmp	??loop

??noop:
	nop

??out:
      	mov	eax,edi
	sub	eax,[a1stdest]
	jmp	short ??end

??notlength:
	mov	cl,al		; get the entire code
	and	ecx,3Fh		; and off all but the size -3
	add	ecx,3		; add 3 for byte count

	cmp	al,0FEh
	jne	short ??notrunlength

	sub	eax,eax
	lodsw
	mov	ecx,eax
	sub	eax,eax
	lodsb
	mov	ebx,esi		;save the source offset

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??runlenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??runlenok:
	rep	stosb
	jmp	??loop

??notrunlength:
	cmp	al,0FFh		; is it a long run?
	jne	short ??notlong	; if not use the code as the size

	sub	eax,eax
	lodsw		; if so, get the size
	mov	ecx,eax

??notlong:
	lodsw			;get the real index
	mov	ebx,esi		;save the source offset
	add	eax,[a1stdest]	;add in the 1st index
	mov	esi,eax		;use eax as new source
	cmp	ecx,[maxlen]	;compare for overrun
	jbe	short ??runok	; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??runok:
	rep	movsb
	jmp	??loop

??end:
	mov	eax,edi
	mov	ebx,[dest]
	sub	eax,ebx
	jmp ??exit

??rel:
	lodsb
	mov	[a1stdest],edi
	add	edx,edi
	mov	[lastbyte],edx
	cld			; make sure all lod and sto are forward
	mov	ebx,esi		; save the source offset

??rel_loop:
	mov	eax,[lastbyte]
	sub	eax,edi		; get the remaining byte to uncomp
	jz	short ??rel_out		; were done

	mov	[maxlen],eax	; save for string commands
	mov	esi,ebx		; mov in the source index
	sub	eax,eax
	lodsb
	or	al,al		; see if its a short run
	js	short ??rel_notshort

	mov	ah,al		; put rel offset high nibble in ah
	and	ah,0Fh		; only 4 bits count
	sub	ecx,ecx
	mov	cl,al		;put count nibble in ch
	shr	cl,4		; get run -3
	add	ecx,3		; get actual run length

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??rel_rsok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rel_rsok:
	lodsb			; get rel offset low byte
	mov	ebx,esi		; save the source offset
	mov	esi,edi		; get the current dest
	sub	esi,eax		; get relative offset

	rep	movsb

	jmp	??rel_loop

??rel_notshort:
	test	al,40h		; is it a length?
	jne	short ??rel_notlength	; if not it could be med or long run

	cmp	al,80h		; is it the end?
	je	short ??rel_out		; if so its over

	mov	cl,al		; put the byte in count register
	and	ecx,3Fh		; and off the extra bits

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??rel_lenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rel_lenok:
	rep movsb

	mov	ebx,esi		; save the source offset
	jmp	??rel_loop

??rel_out:
      	mov	eax,edi
	sub	eax,[a1stdest]
	jmp	short ??end

??rel_notlength:
	mov	cl,al		; get the entire code
	and	ecx,3Fh		; and off all but the size -3
	add	ecx,3		; add 3 for byte count

	cmp	al,0FEh
	jne	short ??rel_notrunlength

	sub	eax,eax
	lodsw
	mov	ecx,eax
	sub	eax,eax
	lodsb
	mov	ebx,esi		;save the source offset

	cmp	ecx,[maxlen]	; is it too big to fit?
	jbe	short ??rel_runlenok		; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rel_runlenok:
	rep	stosb
	jmp	??rel_loop

??rel_notrunlength:
	cmp	al,0FFh		; is it a long run?
	jne	short ??rel_notlong	; if not use the code as the size

	sub	eax,eax
	lodsw		; if so, get the size
	mov	ecx,eax

??rel_notlong:
	lodsw			;get the real index
	mov	ebx,esi		;save the source offset
	mov	esi,edi
	sub	esi,eax		;use eax as new source
	cmp	ecx,[maxlen]	;compare for overrun
	jbe	short ??rel_runok	; if not, its ok

	mov	ecx,[maxlen]	; if so, max it out so it dosen't overrun

??rel_runok:
	rep	movsb
	jmp	??rel_loop

	jmp	??end

??exit:
	ret

VQA_LCW_Uncompress endp

;***********************************************************


	END