
;    This file is part of the q-objc runtime library.
;    Copyright (C) 2008-2009 Taras Tovchenko Victorovich <doctorset@gmail.com>
;    
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;    
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;    
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.


.386
.model flat, c
option casemap:none

SAVE_TOP_OF_STACK macro	copyDwordCnt
	mov		esi, esp
	call	q_retainMemBlock
	mov		edi, eax
	mov		ecx, copyDwordCnt
	cld
	rep		movsd
endm

RESTORE_TOP_OF_STACK macro
ex:	call	q_releaseMemBlock
	mov		esi, eax
	mov		edi, esp
	mov		ecx, kUpStackNum
	cld
	rep		movsd
	mov		eax, dword ptr [retVal]
	mov		edx, dword ptr [retVal + sizeof DWORD]
endm

CORRECT_FLOAT_VALUES macro argStackBytes
	mov		ebx, esp
	add		ebx, argStackBytes
	push	methodTypes
	push	ebx
	call	q_correctFloatValues
	add		esp, 8
	mov		shiftCnt, eax
	call	q_currentMemBlock
	mov		ebx, dword ptr [shiftCnt]
	mov		dword ptr [eax + argStackBytes], ebx
endm

LOOKUP_IMPLEMENTATION macro argOffset, lookupFunc
	CHECK_OBJECT_ON_NIL	argOffset
	push	offset methodTypes
	push	dword ptr [ebp + 0Ch + argOffset]	; 2nd arg - selector
	push	dword ptr [ebp + 8 + argOffset]		; 1st arg - object
	call	lookupFunc
	add		esp, 0Ch
	mov		impl, eax
endm

INVOKE_IMPLEMENTATION macro
	add		esp, dword ptr [shiftCnt]
	add		esp, kUpStackBytes
	call	impl								; Save result
	mov		dword ptr [retVal], eax
	mov		dword ptr [retVal + sizeof DWORD], edx
	call	q_currentMemBlock					; Update shiftCnt variable
	mov		ebx, dword ptr [eax + kUpStackBytes]
	mov		dword ptr [shiftCnt], ebx
	sub		esp, kUpStackBytes
	sub		esp, dword ptr [shiftCnt]
endm

INVOKE_IMPLEMENTATION_ST macro
	add		esp, dword ptr [shiftCnt]
	add		esp, kUpStackBytesWithFirstArg
	cmp		byte ptr [is2MachineWordSize], 1
	jnz		m1
	
	call	q_currentMemBlock					; CALL this body when size of struct > 2 * MACHINE_WORD_SIZE (saved ADDRESS)
	push	dword ptr [eax + kUpStackBytes]		; put address for return
	call	impl
	add		esp, sizeof DWORD
	jmp		m2
	
m1:	cmp		byte ptr [is1MachineWordSize], 1
	jnz		m3
	
	call	impl								; CALL this body when size of struct > MACHINE_WORD_SIZE (EAX:EDX)
	mov		dword ptr [retVal], eax				; Save result
	mov		dword ptr [retVal + sizeof DWORD], edx
	
	call	q_currentMemBlock
	add		eax, kUpStackBytes
	mov		ebx, dword ptr [retVal]
	mov		eax, dword ptr [eax]
	mov		dword ptr [eax], ebx
	mov		ebx, dword ptr [retVal + sizeof DWORD]
	mov		dword ptr [eax + sizeof DWORD], ebx
	jmp		m2
	
m3: call	impl								; CALL this body when size of struct <= MACHINE_WORD_SIZE (EAX)
	mov		dword ptr [retVal], eax				; Save result
	
	call	q_currentMemBlock
	add		eax, kUpStackBytes
	mov		ebx, dword ptr [retVal]
	mov		eax, dword ptr [eax]
	mov		dword ptr [eax], ebx
	
m2:	call	q_currentMemBlock					; Update shiftCnt variable
	mov		ebx, dword ptr [eax + kUpStackBytesWithFirstArg]
	mov		dword ptr [shiftCnt], ebx
	sub		esp, kUpStackBytesWithFirstArg
	sub		esp, dword ptr [shiftCnt]
endm

IS_MORE_BYTES macro
	push	dword ptr [methodTypes]
	call	q_retSize
	add		esp, sizeof DWORD
	cmp		eax, k1MachineWordSize
	ja		b1							; IF size > MACHINE_WORD_SIZE
	mov		byte ptr [is1MachineWordSize], 0
	jmp		b2
b1:	mov		byte ptr [is1MachineWordSize], 1
	
	cmp		eax, k2MachineWordSize
	ja		b3							; IF size > 2 * MACHINE_WORD_SIZE
b2:	mov		byte ptr [is2MachineWordSize], 0
	jmp		b4
b3: mov		byte ptr [is2MachineWordSize], 1
b4: 
endm

CHECK_OBJECT_ON_NIL macro argOffset
	cmp		dword ptr [ebp + 8 + argOffset], 0
	jnz		n1
	mov		dword ptr [retVal], 0
	mov		dword ptr [retVal + sizeof DWORD], 0
	jmp		ex							; IF object == nil go to exit
n1:
endm

CHECK_RETVAL_ON_NIL macro
	cmp		dword ptr [ebp + 8], 0
	jnz		go
	push	kSendStError
	call	q_throwError
go:
endm


kUpStackNum					equ 5
kUpStackBytes				equ kUpStackNum * sizeof DWORD
kUpStackBytesWithFirstArg	equ	kUpStackBytes + sizeof DWORD
k1MachineWordSize			equ sizeof DWORD
k2MachineWordSize			equ	2 * sizeof DWORD

.data?
methodTypes				dd	?
impl					dd	?
shiftCnt				dd	?
retVal					dd	2 dup(?)
is1MachineWordSize		db	?
is2MachineWordSize		db	?

.code
extern q_retainMemBlock:NEAR
extern q_releaseMemBlock:NEAR
extern q_currentMemBlock:NEAR
extern q_lookupImp:NEAR
extern q_lookupImpSuper:NEAR
extern q_correctFloatValues:NEAR
extern q_retSize:NEAR
extern q_throwError:NEAR
extern kSendStError:DWORD

public q_send
public q_sendSt
public q_sendSuper
public q_sendStSuper

q_send proc
	push	ebp
	mov		ebp, esp
	
	push	ebx
	push	esi
	push	edi
	
	finit
	
	SAVE_TOP_OF_STACK		kUpStackNum
	LOOKUP_IMPLEMENTATION	0, q_lookupImp
	CORRECT_FLOAT_VALUES	kUpStackBytes
	INVOKE_IMPLEMENTATION
	RESTORE_TOP_OF_STACK
	
	pop		edi
	pop		esi
	pop		ebx
	
	mov		esp, ebp
	pop ebp
	ret
q_send endp

q_sendSt proc
	push	ebp
	mov		ebp, esp
	
	push	ebx
	push	esi
	push	edi
	
	finit
	
	CHECK_RETVAL_ON_NIL
	SAVE_TOP_OF_STACK		kUpStackNum + 1
	LOOKUP_IMPLEMENTATION	sizeof DWORD, q_lookupImp
	IS_MORE_BYTES
	CORRECT_FLOAT_VALUES	kUpStackBytesWithFirstArg
	INVOKE_IMPLEMENTATION_ST
	RESTORE_TOP_OF_STACK
	
	pop		edi
	pop		esi
	pop		ebx
	
	mov		esp, ebp
	pop ebp
	ret
q_sendSt endp

q_sendSuper proc
	push	ebp
	mov		ebp, esp
	
	push	ebx
	push	esi
	push	edi
	
	finit
	
	SAVE_TOP_OF_STACK		kUpStackNum
	LOOKUP_IMPLEMENTATION	0, q_lookupImpSuper
	CORRECT_FLOAT_VALUES	kUpStackBytes
	INVOKE_IMPLEMENTATION
	RESTORE_TOP_OF_STACK
	
	pop		edi
	pop		esi
	pop		ebx
	
	mov		esp, ebp
	pop ebp
	ret
q_sendSuper endp

q_sendStSuper proc
	push	ebp
	mov		ebp, esp
	
	push	ebx
	push	esi
	push	edi
	
	finit
	
	CHECK_RETVAL_ON_NIL
	SAVE_TOP_OF_STACK		kUpStackNum + 1
	LOOKUP_IMPLEMENTATION	sizeof DWORD, q_lookupImpSuper
	IS_MORE_BYTES
	CORRECT_FLOAT_VALUES	kUpStackBytesWithFirstArg
	INVOKE_IMPLEMENTATION_ST
	RESTORE_TOP_OF_STACK
	
	pop		edi
	pop		esi
	pop		ebx
	
	mov		esp, ebp
	pop ebp
	ret
q_sendStSuper endp

end