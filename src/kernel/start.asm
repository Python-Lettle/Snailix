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
halt:
    mov byte [0xb80a0], 'E'
    mov byte [0xb80a1], 0x0f
    mov byte [0xb80a2], 'r'
    mov byte [0xb80a3], 0x0f
    mov byte [0xb80a4], 'r'
    mov byte [0xb80a5], 0x0f
    mov byte [0xb80a6], 'o'
    mov byte [0xb80a7], 0x0f
    mov byte [0xb80a8], 'r'
    mov byte [0xb80a9], 0x0f
    mov byte [0xb80aa], '!'
    mov byte [0xb80ab], 0x0f
    hlt
    jmp $

section .data

code_selector equ (1 << 3)
data_selector equ (2 << 3)
magic equ 0x22CAFE33