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

;****************************************************************************
;*
;*          Copyright (c) 1994, HMI, INC. All Rights Reserved
;*
;*---------------------------------------------------------------------------
;*
;* FILE
;*     soscodec.asm
;*
;* DESCRIPTION
;*     HMI SOS ADPCM compression/decompression.
;*
;* PROGRAMMER
;*     Nick Skrepetos
;*     Denzil E. Long, Jr. (Fixed bugs, rewrote for watcom)
;*	   Bill Petro		   (Added stereo support)
;* DATE
;*     Febuary 15, 1995
;*
;*---------------------------------------------------------------------------
;*
;* PUBLIC
;*
;****************************************************************************

	; IDEAL
	.386P ;P386
	.MODEL FLAT, C ;MODEL	USE32 FLAT
	;LOCALS	??


DPMI_INTR		equ	31h
IF_LOCKED_PM_CODE	equ	1h		; Locked PM code for DPMI.
IF_LOCKED_PM_DATA	equ	2h		; Locked PM code for DPMI.

	sCompInfo STRUC
lpSource		DD	?	;Compressed data pointer
lpDest			DD	?	;Uncompressed data pointer
dwCompSize		DD	?	;Compressed size
dwUnCompSize		DD	?	;Uncompressed size

dwSampleIndex		DD	?	;Index into sample
dwPredicted		DD	?	;Next predicted value
dwDifference		DD	?	;Difference from last sample
wCodeBuf		DW	?	;Holds 2 nibbles for decompression
wCode			DW	?	;Current 4 bit code
wStep			DW	?	;Step value in table
wIndex			DW	?	;Index into step table

dwSampleIndex2		DD	?	;Index into sample
dwPredicted2		DD	?	;Next predicted value
dwDifference2		DD	?	;Difference from last sample
wCodeBuf2 		DW	?	;Holds 2 nibbles for decompression
wCode2			DW	?	;Current 4 bit code
wStep2			DW	?	;Step value in table
wIndex2		DW	?	;Index into step table

wBitSize		DW	?	;Bit size for decompression
wChannels		DW	?    	;number of channels
	sCompInfo	ENDS

	.DATA ;DATASEG


;InitFlags	DD	0		; Flags to indicate what has been initialized.


;LABEL	LockedDataStart	BYTE

;* Index table for stepping into step table

wCODECIndexTab	DW	-1,-1,-1,-1,2,4,6,8
		DW	-1,-1,-1,-1,2,4,6,8


;Lookup table of replacement values
;The actual sound value is replaced with an index to lookup in this table
;The index only takes up a nibble(4bits) and represents an int(16bits)
;Essentially:
;Get a value
;compare it with the value before it
;find closest value in table and store the index into the table
;if i'm going down then negitize it
;go to next byte.

;Theory for stereo:
;1)handle stereo and mono in two seperate loops. cleaner...
;start at byte 0 and skip every other byte(or word) both write and read
;when we get done
;	set start byte to 1 and do it again




;This table essentialy round off to closes values in 3 distinct bands
; precalculated and optimized(i guess) for human hearing.

wCODECStepTab	DW	7,  	8,  	9,   	10,   	11,  	12,   	13,14
	DW	16, 	17, 	19, 	21,  	23,   	25,  	28,   	31
	DW	34, 	37, 	41, 	45,  	50,   	55,  	60,   	66
	DW	73, 	80, 	88, 	97,  	107,  	118, 	130,  	143
	DW	157,	173,	190,	209, 	230,  	253, 	279,  	307
	DW	337,	371,	408,	449, 	494,  	544, 	598,  	658
	DW	724,	796,	876,	963, 	1060,	1166,	1282,	1411
	DW	1552,	1707,	1878,	2066,	2272,	2499,	2749,	3024
	DW	3327,	3660,	4026,	4428,	4871,	5358,	5894,	6484
	DW	7132,	7845,	8630,	9493,	10442,	11487,	12635,	13899
	DW	15289,	16818,	18500,	20350,	22385,	24623,	27086,	29794
	DW	32767

;dwCODECByteIndex		DD	0 ; this is when to stop compressing
;dwCODECTempStep		DD	0 ; tempory storage for step value
;wCODECMask			DW	0 ; Current mask

;LABEL	LockedDataEnd	BYTE


	.CODE ;CODESEG

;LABEL	LockedCodeStart	BYTE

;****************************************************************************
;*
;* NAME
;*     sosCODECInitStream - Initialize compression stream.
;*
;* SYNOPSIS
;*     sosCODECInitStream(CompInfo)
;*
;*     void sosCODECInitStream(_SOS_COMPRESS_INFO *);
;*
;* FUNCTION
;*     Initialize compression stream for compression and decompression.
;*
;* INPUTS
;*     CompInfo - Compression information structure.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC General_sosCODECInitStream
; FUNCTION: GAMEMD 0x007C6270
	General_sosCODECInitStream PROC NEAR \
	\
	sSOSInfo:DWORD

	assume eax:ptr sCompInfo

	mov	eax,[sSOSInfo]
	mov	[[eax].wIndex],0 		; starting index 0
	mov	[[eax].wStep],7  		; start with a step of 7
	mov	[[eax].dwPredicted],0 	; no predicted value
	mov	[[eax].dwSampleIndex],0	;start at head of index
	mov	[[eax].wIndex2],0 		; starting index 0
	mov	[[eax].wStep2],7  		; start with a step of 7
	mov	[[eax].dwPredicted2],0 	; no predicted value
	mov	[[eax].dwSampleIndex2],0 	;start at head of index
	ret

	General_sosCODECInitStream ENDP



;****************************************************************************
;*
;* NAME
;*     sosCODECDecompressData - Decompress audio data.
;*
;* SYNOPSIS
;*     Size = sosCODECDecompressData(CompInfo, NumBytes)
;*
;*     long sosCODECDecompressData(_SOS_COMPRESS_INFO *, long);
;*
;* FUNCTION
;*     Decompress data from a 4:1 ADPCM compressed stream. The number of
;*     bytes decompressed is returned.
;*
;* INPUTS
;*     CompInfo - Compress information structure.
;*     NumBytes - Number of bytes to compress.
;*
;* RESULT
;*     Size - Size of decompressed data.
;*
;****************************************************************************

	PUBLIC General_sosCODECDecompressData
; FUNCTION: GAMEMD 0x007C62AC
	General_sosCODECDecompressData PROC NEAR \
	\
	sSOSInfo:DWORD, \
	wBytes:DWORD

	assume ebx:ptr sCompInfo

	local	dwCODECBytesProcessed:dword             ;bytes to decompress
	local	dwCODECByteIndex:dword			;this is when to stop compressing
							; these need to be local if the function is to be reenterant
	push	esi
	push	edi
	push	ebx
	push	ecx
	push	edx

;*---------------------------------------------------------------------------
;*	Initialize
;*---------------------------------------------------------------------------

	mov	ebx,[sSOSInfo]
	mov	eax,[wBytes]
	mov	[dwCODECBytesProcessed],eax
	mov	[[ebx].dwSampleIndex],0 	;start at head of index
	mov	[[ebx].dwSampleIndex2],0 	;start at head of index

							;Check for 16 bit decompression

	cmp	[[ebx].wBitSize],16
	jne	short ??skipByteDivide

	shr	eax,1	;Divide size by two

??skipByteDivide:
	mov	[dwCODECByteIndex],eax
	mov	esi,[[ebx].lpSource]
	mov	edi,[[ebx].lpDest]
	cmp	[[ebx].wChannels],2		;stereo check
	je	??mainloopl				;do left side first

;	Determine if sample index is even or odd. This will determine
;	if we need to get a new token or not.

;---------------------------------------------------------------------------
;Main Mono Loop
;---------------------------------------------------------------------------



??mainloop:
	test	[[ebx].dwSampleIndex],1	;odd ??
	je	short ??fetchToken			; if so get new token
	xor	eax,eax					;else shift int codebuf
	mov	ax,[[ebx].wCodeBuf]		;ored with Code
	shr	eax,4
	and	eax,000Fh
	mov	[[ebx].wCode],ax
	jmp	short ??calcDifference

??fetchToken:
	xor	eax,eax					;get a new token
	mov	al,[esi]				;put in codebuf
	mov	[[ebx].wCodeBuf],ax
	inc	esi
	and	eax,000Fh
	mov	[[ebx].wCode],ax		;and then code

??calcDifference:
	mov	[[ebx].dwDifference],0	;reset diff
	xor	ecx,ecx
	mov	cx,[[ebx].wStep]		;cx is step value
	test	eax,4					;Check for wCode & 4
	je	short ??no4
	add	[[ebx].dwDifference],ecx	;Add wStep

??no4:
	test	eax,2					;Check for wCode & 2
	je	short ??no2
	mov	edx,ecx					;Add wStep >> 1
	shr	edx,1
	add	[[ebx].dwDifference],edx

??no2:
	test	eax,1					;Check for wCode & 1
	je	short ??no1
	mov	edx,ecx					;Add wStep >> 2
	shr	edx,2
	add	[[ebx].dwDifference],edx

??no1:
	mov	edx,ecx					;Add in wStep >> 3
	shr	edx,3
	add	[[ebx].dwDifference],edx
	test	eax,8					;Check for wCode & 8
	je	short ??no8
	neg	[[ebx].dwDifference]		;Negate diff

??no8:
	; add difference to predicted value.

	mov	eax,[[ebx].dwPredicted]
	add	eax,[[ebx].dwDifference]

	; make sure there is no under or  overflow.

	cmp	eax,7FFFh
	jl	short ??noOverflow
	mov	eax,7FFFh

??noOverflow:
	cmp	eax,0FFFF8000h
	jg	short ??noUnderflow
	mov	eax,0FFFF8000h

??noUnderflow:
	mov	[[ebx].dwPredicted],eax
	cmp	[[ebx].wBitSize],16
	jne	short ??output8Bit
	mov	[edi],ax				;Output 16bit sample
	add	edi,2
	jmp	short ??adjustIndex

??output8Bit:
	; output 8 bit sample

	xor	ah,80h
	mov	[edi],ah
	inc	edi

??adjustIndex:
	xor	ecx,ecx
	mov	cx,[[ebx].wCode]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECIndexTab + ecx]
	add	[[ebx].wIndex],ax 		;check if wIndex < 0
	cmp	[[ebx].wIndex],8000h
	jb	short ??checkOverflow
	mov	[[ebx].wIndex],0		;reset index to zero
	jmp	short ??adjustStep

??checkOverflow:
	cmp	[[ebx].wIndex],88		;check if wIndex > 88
	jbe	short ??adjustStep
	mov	[[ebx].wIndex],88		;reset index to 88

??adjustStep:
	; fetch wIndex so we can fetch new step value

	xor	ecx,ecx
	mov	cx,[[ebx].wIndex]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECStepTab + ecx]

	; advance index and store step value

	add	[[ebx].dwSampleIndex],1
	mov	[[ebx].wStep],ax

	; decrement bytes processed and loop back.

	dec	[dwCODECByteIndex]
	jne	??mainloop
	jmp	??exitout

;--------------------------------------------------------------------------
;Left Channel Start
;--------------------------------------------------------------------------


??mainloopl:
	test	[[ebx].dwSampleIndex],1
	je	short ??fetchTokenl

	xor	eax,eax
	mov	ax,[[ebx].wCodeBuf]
	shr	eax,4
	and	eax,000Fh
	mov	[[ebx].wCode],ax
	jmp	short ??calcDifferencel

??fetchTokenl:
	xor	eax,eax
	mov	al,[esi]
	mov	[[ebx].wCodeBuf],ax
	add	esi,2					;2 for stereo
	and	eax,000Fh
	mov	[[ebx].wCode],ax

??calcDifferencel:
							; reset difference

	mov	[[ebx].dwDifference],0
	xor	ecx,ecx
	mov	cx,[[ebx].wStep]
	test	eax,4					;Check for wCode & 4
	je	short ??no4l
	add	[[ebx].dwDifference],ecx	;Add wStep

??no4l:
	test	eax,2					;Check for wCode & 2
	je	short ??no2l
	mov	edx,ecx					;Add wStep >> 1
	shr	edx,1
	add	[[ebx].dwDifference],edx

??no2l:
	test	eax,1					;Check for wCode & 1
	je	short ??no1l
	mov	edx,ecx					;Add wStep >> 2
	shr	edx,2
	add	[[ebx].dwDifference],edx

??no1l:
	mov	edx,ecx					;Add in wStep >> 3
	shr	edx,3
	add	[[ebx].dwDifference],edx
	test	eax,8					;Check for wCode & 8
	je	short ??no8l
	neg	[[ebx].dwDifference]		;Negate diff

??no8l:
		; add difference to predicted value.

	mov	eax,[[ebx].dwPredicted]
	add	eax,[[ebx].dwDifference]

		; make sure there is no under or  overflow.

	cmp	eax,7FFFh
	jl	short ??noOverflowl
	mov	eax,7FFFh

??noOverflowl:
	cmp	eax,0FFFF8000h
	jg	short ??noUnderflowl
	mov	eax,0FFFF8000h

??noUnderflowl:
	mov	[[ebx].dwPredicted],eax
	cmp	[[ebx].wBitSize],16
	jne	short ??output8Bitl
	mov	[edi],ax				;Output 16bit sample
	add	edi,4					;4 for stereo
	jmp	short ??adjustIndexl

??output8Bitl:
	; output 8 bit sample

	xor	ah,80h
	mov	[edi],ah
	add	edi,2					;2 for stereo

??adjustIndexl:
	xor	ecx,ecx
	mov	cx,[[ebx].wCode]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECIndexTab + ecx]
	add	[[ebx].wIndex],ax
							; check if wIndex < 0
	cmp	[[ebx].wIndex],8000h
	jb	short ??checkOverflowl
	mov	[[ebx].wIndex],0
	jmp	short ??adjustStepl			;reset index to zero


??checkOverflowl:

	cmp	[[ebx].wIndex],88		; check if wIndex > 88
	jbe	short ??adjustStepl
	mov	[[ebx].wIndex],88		; reset index to 88

??adjustStepl:
	; fetch wIndex so we can fetch new step value

	xor	ecx,ecx
	mov	cx,[[ebx].wIndex]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECStepTab + ecx]

	; advance index and store step value

	add	[[ebx].dwSampleIndex],1
	mov	[[ebx].wStep],ax

	; decrement bytes processed and loop back.

	sub	[dwCODECByteIndex],2
	jne	??mainloopl
;----------------------------------------------------------------------------
; Right Side Setup
;----------------------------------------------------------------------------
	mov	eax,[wBytes]
	mov	[dwCODECBytesProcessed],eax
	mov	esi,[[ebx].lpSource]
	mov	edi,[[ebx].lpDest]
	inc	esi					; skip left channel
	inc	edi				     	; skip left channel
	cmp	[[ebx].wBitSize],16		;16 bit ??
	je	short ??doByteDivide
	mov	[dwCODECByteIndex],eax
	jmp  short ??mainloopr

??doByteDivide:
	shr	eax,1 					;Divide size by two
	inc	edi   					; 16 bit so skip 1 more
	mov	[dwCODECByteIndex],eax


;--------------------------------------------------------------------------
;Right Channel Start
;--------------------------------------------------------------------------


??mainloopr:
	test	[[ebx].dwSampleIndex2],1
	je	short ??fetchTokenr
	xor	eax,eax
	mov	ax,[[ebx].wCodeBuf2]
	shr	eax,4
	and	eax,000Fh
	mov	[[ebx].wCode2],ax
	jmp	short ??calcDifferencer

??fetchTokenr:
	xor	eax,eax
	mov	al,[esi]
	mov	[[ebx].wCodeBuf2],ax
	add	esi,2				     	;2 for stereo
	and	eax,000Fh
	mov	[[ebx].wCode2],ax

??calcDifferencer:
							; reset difference

	mov	[[ebx].dwDifference2],0
	xor	ecx,ecx
	mov	cx,[[ebx].wStep2]
	test	eax,4					;Check for wCode & 4
	je	short ??no4r
	add	[[ebx].dwDifference2],ecx	;Add wStep

??no4r:
	test	eax,2					;Check for wCode & 2
	je	short ??no2r
	mov	edx,ecx					;Add wStep >> 1
	shr	edx,1
	add	[[ebx].dwDifference2],edx

??no2r:
	test	eax,1					;Check for wCode & 1
	je	short ??no1r
	mov	edx,ecx					;Add wStep >> 2
	shr	edx,2
	add	[[ebx].dwDifference2],edx

??no1r:
	mov	edx,ecx					;Add in wStep >> 3
	shr	edx,3
	add	[[ebx].dwDifference2],edx
	test	eax,8					;Check for wCode & 8
	je	short ??no8r
	neg	[[ebx].dwDifference2]		;Negate diff

??no8r:
	; add difference to predicted value.
	mov	eax,[[ebx].dwPredicted2]
	add	eax,[[ebx].dwDifference2]
	cmp	eax,7FFFh
	jl	short ??noOverflowr
	mov	eax,7FFFh

??noOverflowr:
	cmp	eax,0FFFF8000h
	jg	short ??noUnderflowr
	mov	eax,0FFFF8000h

??noUnderflowr:
	mov	[[ebx].dwPredicted2],eax
	cmp	[[ebx].wBitSize],16
	jne	short ??output8Bitr
	mov	[edi],ax				;Output 16bit sample
	add	edi,4					;4 for stereo		***
	jmp	short ??adjustIndexr

??output8Bitr:
							; output 8 bit sample
	xor	ah,80h
	mov	[edi],ah
	add	edi,2					;2 for stereo

??adjustIndexr:
	xor	ecx,ecx
	mov	cx,[[ebx].wCode2]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECIndexTab + ecx]
	add	[[ebx].wIndex2],ax
							; check if wIndex < 0
	cmp	[[ebx].wIndex2],8000h
	jb	short ??checkOverflowr
							; reset index to zero
	mov	[[ebx].wIndex2],0
	jmp	short ??adjustStepr

??checkOverflowr:
							; check if wIndex > 88
	cmp	[[ebx].wIndex2],88
	jbe	short ??adjustStepr
	mov	[[ebx].wIndex2],88		; reset index to 88

??adjustStepr:
	; fetch wIndex so we can fetch new step value

	xor	ecx,ecx
	mov	cx,[[ebx].wIndex2]
	xor	eax,eax
	shl	ecx,1
	mov	ax,[wCODECStepTab + ecx]

	; advance index and store step value

	add	[[ebx].dwSampleIndex2],1
	mov	[[ebx].wStep2],ax

	; decrement bytes processed and loop back.

	sub	[dwCODECByteIndex],2
	jne	??mainloopr


??exitout:
;		don't think we need this but just in case i'll leave it here!!

;	mov	[[ebx].lpSource],esi
;	mov	[[ebx].lpDest],edi
	; set up return value for number of bytes processed.
	mov	eax,[dwCODECBytesProcessed]
	pop	edx
	pop	ecx
	pop	ebx
	pop	edi
	pop	esi
	ret

	General_sosCODECDecompressData ENDP

	END
