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

.386
.model flat, C

PHARLAP_TNT equ 0
VQABLOCK_4X2	EQU	1	;4x2 block decode enable/disable
VQABLOCK_4X4	EQU	1	;4x4 block decode enable/disable

;----------------------------------------------------------------------------
;
; These are VQA UnVQ1 full-frame decoders -- assembly-optimized routines
; for drawing VQ-compressed video frames into a pixel buffer.
;
; Naming convention from VQALib:
;   UnVQ1 = full-frame decoder (processes every pointer)
;   C0    = ColorMode 0 (8-bit palette, 1 byte/pixel)
;   C1    = ColorMode 1 (15-bit hicolor via HicolorTable lookup)
;   TABLE = uses HicolorTable for single-color block fill
;   HALF  = half-resolution output (samples every-other pixel)
;
; Function map (10 functions, all share the same signature):
;
;   Name                    | Block | ColorMode | 1-Color Source      | Block Type Check
;   ASM_UnVQ1_C1_TABLE      | 4x4   | C1 table  | HicolorTable[idx]   | test bh, 80h
;   ASM_UnVQ1_C1_TABLE_ALT  | 4x2   | C1 table  | HicolorTable[idx]   | test bh, 80h
;   ASM_UnVQ_4x2            | 4x2   | C0 (8bit) | byte duplication    | cmp bh, 0FFh
;   ASM_UnVQ_4x4            | 4x4   | C0 (8bit) | byte duplication    | cmp bh, 0FFh
;   ASM_UnVQ_4x4_HALF       | 4x4>2 | C0 half   | byte duplication    | cmp bh, 0FFh
;   ASM_UnVQ_6              | 4x2>2 | C0 half   | byte duplication    | cmp bh, 0FFh
;   ASM_UnVQ1_C1_4x4        | 4x4   | C1 direct | value & 0x7FFF      | test bh, 80h
;   ASM_UnVQ_8              | 4x2   | C1 direct | value & 0x7FFF      | test bh, 80h
;   ASM_UnVQ_9              | 4x8   | C0 (8bit) | byte duplication    | cmp bh, 0FFh
;   ASM_UnVQ_10             | 4x4w  | C0 (8bit) | byte duplication    | cmp bh, 0FFh
;
; Paired functions (same logic, different color source):
;   ASM_UnVQ1_C1_TABLE <-> ASM_UnVQ1_C1_4x4   (4x4, table vs direct)
;   ASM_UnVQ1_C1_TABLE_ALT <-> ASM_UnVQ_8     (4x2, table vs direct)
;   ASM_UnVQ_4x2 <-> ASM_UnVQ_6               (4x2 full vs half-res)
;   ASM_UnVQ_4x4 <-> ASM_UnVQ_4x4_HALF        (4x4 full vs half-res)
;   ASM_UnVQ_9 <-> ASM_UnVQ_10                 (large: 4x8 vs 4x4-wide)
;
; HicolorTable (global): 15-bit RGB to 16-bit pixel lookup table.
; Initialized by Hicolor_Init_Table() in unvqtblc.cpp.
;----------------------------------------------------------------------------

externdef C ASM_UnVQ1_C1_TABLE:NEAR
externdef C ASM_UnVQ1_C1_TABLE_ALT:NEAR
externdef C ASM_UnVQ_4x2:NEAR
externdef C ASM_UnVQ_4x4:NEAR
externdef C ASM_UnVQ_4x4_HALF:NEAR
externdef C ASM_UnVQ_6:NEAR
externdef C ASM_UnVQ1_C1_4x4:NEAR
externdef C ASM_UnVQ_8:NEAR
externdef C ASM_UnVQ_9:NEAR
externdef C ASM_UnVQ_10:NEAR
EXTERN C HicolorTable:DWORD ; USHORT * [65535-1];

.CODE

;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ1_C1_TABLE - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ1_C1_TABLE(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ1_C1_TABLE(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ1_C1_TABLE
; FUNCTION: GAMEMD 0x007C6B70
	ASM_UnVQ1_C1_TABLE PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	unknown:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	shl	[bufwidth],1
	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,2		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	test	bh,080h		;Is it a one color block?
	js	short ??One_color

;	Draw multi-color block

	shl	ebx,5
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword

	mov	[edi],eax
	mov	[edi+4],edx

	mov	eax,[ebx+8]
	mov	edx,[ebx+12]
	mov	[unknown],ebx
	mov	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	mov	ebx,[unknown]
	mov	eax,[ebx+16]
	mov	edx,[ebx+20]
	mov	ebx,[bufwidth]
	shl	ebx,1
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	mov	ebx,[unknown]
	mov	eax,[ebx+24]
	mov	edx,[ebx+28]
	mov	ebx,[bufwidth]
	shl	ebx,1
	add	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	and	ebx,07FFFh
	shl	ebx,1
	mov	edx,[HicolorTable]
	mov	ax,[edx+ebx]
	mov	bx,ax
	shl	eax,16
	mov	ax,bx

	mov	[edi],eax
	mov	[edi+4],eax

	mov	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	edi,8		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	??Not_finished_a_line ;jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ1_C1_TABLE ENDP


;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ1_C1_TABLE_ALT - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ1_C1_TABLE_ALT(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ1_C1_TABLE_ALT(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ1_C1_TABLE_ALT
; FUNCTION: GAMEMD 0x007C6C79
	ASM_UnVQ1_C1_TABLE_ALT PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	unknown:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	shl	[bufwidth],1
	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,2		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	test	bh,080h		;Is it a one color block?
	js	short ??One_color

;	Draw multi-color block

	shl	ebx,5
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword
	mov	[edi],eax
	mov	[edi+4],edx
	mov	eax,[ebx+16]
	mov	edx,[ebx+20]
	mov	ebx,[bufwidth]
	shl	ebx,1
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	and	ebx,07FFFh
	shl	ebx,1
	mov	edx,[HicolorTable]
	mov	ax,[edx+ebx]
	mov	bx,ax
	shl	eax,16
	mov	ax,bx

	mov	[edi],eax
	mov	[edi+4],eax

	mov	ebx,[bufwidth]
	add	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	edi,8		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ1_C1_TABLE_ALT ENDP


	IF	VQABLOCK_4X2
;****************************************************************************
;*
;* NAME
;*     UnVQ_4x2 - Draw 4x2 block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     UnVQ_4x2(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void UnVQ_4x2(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_4x2
; FUNCTION: GAMEMD 0x007C6D40
	ASM_UnVQ_4x2 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,1		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,3
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword
	mov	ebx,[bufwidth]
	mov	[edi],eax		;Write 1st row to dest
	mov	[edi+ebx],edx		;Write 2nd row to dest

	add	edi,4		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.
	rol	eax,16
	mov	ax,bx
	mov	ebx,[bufwidth]
	mov	[edi],eax		;Write 1st row to dest
	mov	[edi+ebx],eax		;Write 2nd row to dest

	add	edi,4		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ_4x2 ENDP
	ENDIF	;VQABLOCK_4X2


	IF	VQABLOCK_4X4
;****************************************************************************
;*
;* NAME
;*     UnVQ_4x4 - Draw 4x4 block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     UnVQ_4x4(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void UnVQ_4x4(unsigned char *, unsigned char *, unsigned char *,
;*                        unsigned short, unsigned short, unsigned short);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_4x4
; FUNCTION: GAMEMD 0x007C6DD6
	ASM_UnVQ_4x4 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,2		; row of blocks
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	IF	PHARLAP_TNT
	push	es
	les	edi,[FWORD buffer]	;KLUDGE - bcc32 pads FARPTR
	ELSE
	mov	edi,[buffer]
	ENDIF

	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,4
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword

	IF	PHARLAP_TNT
	mov	[es:edi],eax		;Write 1st row to dest
	ELSE
	mov	[edi],eax		;Write 1st row to dest
	ENDIF

	mov	eax,ebx
	mov	ebx,[bufwidth]

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],edx	;Write 2nd row to dest
	ELSE
	mov	[edi+ebx],edx		;Write 2nd row to dest
	ENDIF

	mov	ebx,eax
	mov	eax,[ebx+8]
	mov	edx,[ebx+12]
	mov	ebx,[bufwidth]
	shl	ebx,1

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],eax	;Write 2nd row to dest
	ELSE
	mov	[edi+ebx],eax		;Write 2nd row to dest
	ENDIF

	add	ebx,[bufwidth]

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],edx	;Write 2nd row to dest
	ELSE
	mov	[edi+ebx],edx		;Write 2nd row to dest
	ENDIF

	add	edi,4		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.
	rol	eax,16
	mov	ax,bx

	IF	PHARLAP_TNT
	mov	[es:edi],eax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[es:edi+ebx],eax	;Write 2nd row to dest
	ELSE
	mov	[edi],eax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[edi+ebx],eax		;Write 2nd row to dest
	ENDIF

	add	ebx,[bufwidth]

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],eax	;Write 3rd row to dest
	ELSE
	mov	[edi+ebx],eax		;Write 3rd row to dest
	ENDIF

	add	ebx,[bufwidth]

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],eax	;Write 4th row to dest
	ELSE
	mov	[edi+ebx],eax		;Write 4th row to dest
	ENDIF

	add	edi,4		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	IF	PHARLAP_TNT
	pop	es
	ENDIF

	ret

	ASM_UnVQ_4x4 ENDP
	ENDIF	;VQABLOCK_4X4


;;;; this is very close to UnVQ_2x2 in unvqbuff.asm ;;;;
;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ_4x4_HALF - Draw 4x4 block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ_4x4_HALF(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ_4x4_HALF(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_4x4_HALF
; FUNCTION: GAMEMD 0x007C6E94
	ASM_UnVQ_4x4_HALF PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,1		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	IF	PHARLAP_TNT
	push	es
	les	edi,[FWORD buffer]	;KLUDGE - bcc32 pads FARPTR
	ELSE
	mov	edi,[buffer]
	ENDIF

	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,4
	add	ebx,[codebook]	;Codeword address
	mov	al,[ebx]
	mov	ah,[ebx+2]

	IF	PHARLAP_TNT
	mov	[es:edi],ax		;Write 1st row to dest
	ELSE
	mov	[edi],ax		;Write 1st row to dest
	ENDIF

	mov	al,[ebx+8]
	mov	ah,[ebx+10]
	mov	ebx,[bufwidth]

	IF	PHARLAP_TNT
	mov	[es:edi+ebx],ax	;Write 2nd row to dest
	ELSE
	mov	[edi+ebx],ax		;Write 2nd row to dest
	ENDIF


	add	edi,2		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.

	IF	PHARLAP_TNT
	mov	[es:edi],ax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[es:edi+ebx],ax	;Write 2nd row to dest
	ELSE
	mov	[edi],ax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[edi+ebx],ax		;Write 2nd row to dest
	ENDIF

	add	edi,2		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	IF	PHARLAP_TNT
	pop	es
	ENDIF

	ret

	ASM_UnVQ_4x4_HALF ENDP


;;;; this is very close to UnVQ_2x2 in unvqbuff.asm ;;;;
;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ_6 - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ_6(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ_6(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_6
; FUNCTION: GAMEMD 0x007C6F2E
	ASM_UnVQ_6 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,1		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	IF	PHARLAP_TNT
	push	es
	les	edi,[FWORD buffer]	;KLUDGE - bcc32 pads FARPTR
	ELSE
	mov	edi,[buffer]
	ENDIF

	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,3
	add	ebx,[codebook]	;Codeword address
	mov	al,[ebx]
	mov	ah,[ebx+2]

	IF	PHARLAP_TNT
	mov	[es:edi],ax		;Write 1st row to dest
	ELSE
	mov	[edi],ax		;Write 1st row to dest
	ENDIF

	mov	al,[ebx+8]
	mov	ah,[ebx+10]

	IF	PHARLAP_TNT
	mov	ebx,[bufwidth]
	mov	[es:edi+ebx],ax	;Write 2nd row to dest
	ELSE
	mov	ebx,[bufwidth]
	mov	[edi+ebx],ax		;Write 2nd row to dest
	ENDIF


	add	edi,2		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.

	IF	PHARLAP_TNT
	mov	[es:edi],ax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[es:edi+ebx],ax	;Write 2nd row to dest
	ELSE
	mov	[edi],ax		;Write 1st row to dest
	mov	ebx,[bufwidth]
	mov	[edi+ebx],ax		;Write 2nd row to dest
	ENDIF

	add	edi,2		;Next dest block positionw
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ_6 ENDP


;;;; same as UnVQ_1 except fetches from buffer instead of HicolorTable ;;;;
;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ1_C1_4x4 - Draw 4x4 block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ1_C1_4x4(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ1_C1_4x4(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ1_C1_4x4
; FUNCTION: GAMEMD 0x007C6FC8
	ASM_UnVQ1_C1_4x4 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	unknown:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	shl	[bufwidth],1
	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,2		; row of blocks
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	IF	PHARLAP_TNT
	push	es
	les	edi,[FWORD buffer]	;KLUDGE - bcc32 pads FARPTR
	ELSE
	mov	edi,[buffer]
	ENDIF

	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	test	bh,080h		;Is it a one color block?
	js	short ??One_color

;	Draw multi-color block

	shl	ebx,5
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword

	mov	[edi],eax
	mov	[edi+4],edx

	mov	eax,[ebx+8]
	mov	edx,[ebx+12]
	mov	[unknown],ebx
	mov	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	mov	ebx,[unknown]
	mov	eax,[ebx+16]
	mov	edx,[ebx+20]
	mov	ebx,[bufwidth]
	shl	ebx,1
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx
	mov	ebx,[unknown]

	mov	eax,[ebx+24]
	mov	edx,[ebx+28]
	mov	ebx,[bufwidth]
	shl	ebx,1
	add	ebx,[bufwidth]
	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	and	bx,07FFFh
	mov	ax,bx
	shl	eax,16
	mov	ax,bx

	mov	[edi],eax
	mov	[edi+4],eax

	mov	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	??Not_finished_a_line; jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ1_C1_4x4 ENDP


;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ_8 - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ_8(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ_8(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_8
; FUNCTION: GAMEMD 0x007C70C4
	ASM_UnVQ_8 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	unknown:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	shl	[bufwidth],1
	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,1		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bh,[esi + eax]	;Get the codebook pointer value
	mov	bl,[esi]
	inc	esi		; then advance to the next one.

	test	bh,080h		;Is it a one color block?
	js	short ??One_color

;	Draw multi-color block

	shl	ebx,4
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]		;Read 1st row of codeword
	mov	edx,[ebx+4]		;Read 2nd row of codeword

	mov	[edi],eax
	mov	[edi+4],edx

	mov	eax,[ebx+8]
	mov	edx,[ebx+12]

	mov	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],edx

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	and bx,07FFFh
	mov ax,bx
	shl	eax,16
	mov	ax,bx

	mov	[edi],eax
	mov	[edi+4],eax

	mov	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ_8 ENDP


;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ_9 - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ_9(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ_9(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_9
; FUNCTION: GAMEMD 0x007C7174
	ASM_UnVQ_9 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,3		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bl,[esi]
	mov	bh,[esi + eax]	;Get the codebook pointer value
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,5
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]
	mov	edx,[bufwidth]

	mov	[edi],eax

	mov	eax,[ebx+4]
	mov	[edi+edx],eax

	shl	edx,1

	mov	eax,[ebx+8]
	mov	[edi+edx],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+12]
	mov	[edi+edx],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+16]
	mov	[edi+edx],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+20]
	mov	[edi+edx],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+24]
	mov	[edi+edx],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+28]
	mov	[edi+edx],eax

	add	edi,4		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.
	rol	eax,16
	mov	ax,bx
	mov	ebx,[bufwidth]

	mov	[edi],eax

	mov	[edi+ebx],eax

	shl	ebx,1
	mov	[edi+ebx],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax

	add	ebx,[bufwidth]
	mov	[edi+ebx],eax

	add	edi,4		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	??Not_finished_a_line ;jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ_9 ENDP


;****************************************************************************
;*
;* NAME
;*     ASM_UnVQ_10 - Draw XxX block VQ frame to a buffer.
;*
;* SYNOPSIS
;*     ASM_UnVQ_10(Codebook, Pointers, Buffer, BPR, Rows, BufWidth)
;*
;*     void ASM_UnVQ_10(unsigned char *, unsigned char *, unsigned char *,
;*                        long, long, long);
;*
;* FUNCTION
;*     This function draws an image into the specified buffer from the
;*     pointers and codebook provided. This routine has been optimized for
;*     a 320x200 image.
;*
;* INPUTS
;*     Codebook - Pointer to codebook used to draw image.
;*     Pointers - Pointer to vector pointer data.
;*     Buffer   - Pointer to buffer to draw image into.
;*     BPR      - Number of blocks per row.
;*     Rows     - Number of rows.
;*     BufWidth - Width of destination buffer in pixels.
;*
;* RESULT
;*     NONE
;*
;****************************************************************************

	PUBLIC ASM_UnVQ_10
; FUNCTION: GAMEMD 0x007C726A
	ASM_UnVQ_10 PROC C USES ebx ecx edx esi edi \
	\
	codebook:DWORD, \
	pointers:DWORD, \
	buffer:DWORD, \
	blocksperrow:DWORD, \
	numrows:DWORD, \
	bufwidth:DWORD

	LOCAL	data_end:DWORD
	;LOCAL	cb_offset:DWORD
	LOCAL	edi_startval:DWORD
	LOCAL	rowoffset:DWORD
	LOCAL	entries:DWORD

;----------------------------------------------------------------------------
;	Initialize
;----------------------------------------------------------------------------

;	mov	eax,[codebook]		;Adjust the codebook address so
;	sub	eax,4		; that the pointer offsets will
;	mov	[cb_offset],eax	; point directly at the codeword.

	mov	eax,[bufwidth]		;Compute the offset to the next
	shl	eax,2		; row of blocks.
	mov	[rowoffset],eax

	mov	esi,[pointers]
	mov	eax,[numrows]		;Compute the end address of the
	mul	[blocksperrow]		; pointer data.
	mov	[entries],eax
	add	eax,esi
	mov	[data_end],eax

	mov	edi,[buffer]
	mov	[edi_startval],edi

;----------------------------------------------------------------------------
;	Drawing loop
;----------------------------------------------------------------------------

??Start_row:
	mov	ecx,[blocksperrow]	;Number of blocks in a line

??Not_finished_a_line:
	mov	eax,[entries]
	xor	ebx,ebx
	mov	bl,[esi]
	mov	bh,[esi + eax]	;Get the codebook pointer value
	inc	esi		; then advance to the next one.

	cmp	bh,0FFh		;Is it a one color block?
	je	short ??One_color

;	Draw multi-color block

	shl	ebx,5
	add	ebx,[codebook]	;Codeword address
	mov	eax,[ebx]
	mov	edx,[bufwidth]

	mov	[edi],eax

	mov	eax,[ebx+4]
	mov	[edi+4],eax

	mov	eax,[ebx+8]
	mov	[edi+edx],eax

	mov	eax,[ebx+12]
	mov	[edi+edx+4],eax

	shl	edx,1

	mov	eax,[ebx+16]
	mov	[edi+edx],eax

	mov	eax,[ebx+20]
	mov	[edi+edx+4],eax

	add	edx,[bufwidth]

	mov	eax,[ebx+24]
	mov	[edi+edx],eax

	mov	eax,[ebx+28]
	mov	[edi+edx+4],eax

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

;	Draw 1-color block

??One_color:
;	cmp	bx,SKIP_PTR		;Is this a skip block?
;	jne	??Draw_One_Color
;
;	add	edi,4		;Move to next dest block position
;	dec	ecx		;More blocks for this row?
;	jnz	short ??Not_finished_a_line
;	jmp	??Next_row

??Draw_One_Color:
;	not	bx		;NOT pointer value to get color
	mov	bh,bl		;Duplicate color through the
	mov	ax,bx		; entire dword register.
	rol	eax,16
	mov	ax,bx
	mov	ebx,[bufwidth]

	mov	[edi],eax
	mov	[edi+4],eax

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	shl	ebx,1

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	ebx,[bufwidth]

	mov	[edi+ebx],eax
	mov	[edi+ebx+4],eax

	add	edi,8		;Next dest block position
	dec	ecx		;More blocks for this row?
	jnz	??Not_finished_a_line ;jnz	short ??Not_finished_a_line

;	Advance to the next destination row of blocks.

??Next_row:
	mov	edi,[edi_startval]
	add	edi,[rowoffset]
	mov	[edi_startval],edi

	cmp	esi,[data_end]		;Have we reached the end of the
	jnb	short ??End_of_data	; pointers buffer?
	jmp	??Start_row

??End_of_data:
	ret

	ASM_UnVQ_10 ENDP

END
