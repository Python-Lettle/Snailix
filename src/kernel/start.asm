;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-28
; Copyright     : MIT License
; Description   : Where the kernel begins.
;===============================================================================
[bits 32]

section .text

extern kernel_main

global _start
_start:
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x10000

    cmp ecx, magic
    jne halt

    push edx            ; High 8 bit of the total size
    push eax            ; Total size of the memory
    call kernel_main

global halt
halt:
    ; mov byte [0xb8094], 'H'
    ; mov byte [0xb8095], 0x0f
    ; mov byte [0xb8096], 'a'
    ; mov byte [0xb8097], 0x0f
    ; mov byte [0xb8098], 'l'
    ; mov byte [0xb8099], 0x0f
    ; mov byte [0xb809a], 't'
    ; mov byte [0xb809b], 0x0f
    ; mov byte [0xb809c], '!'
    ; mov byte [0xb809d], 0x0f
    ; mov byte [0xb809e], '!'
    ; mov byte [0xb809f], 0x0f
    hlt
    jmp $

section .data

code_selector equ (1 << 3)
data_selector equ (2 << 3)
magic equ 0x22CAFE33