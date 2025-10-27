;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-28
; Copyright     : MIT License
; Description   : 
;     Some useful assembly commands are encapsulated into C language functions.
;===============================================================================
[bits 32]

section .text

; u8 inb(u16 port);
global inb      ; Export inb
inb:
    push ebp
    mov ebp, esp        ; Save stack frame

    xor eax, eax        ; Clear eax
    mov edx, [ebp + 8]  ; port
    in al, dx           ; Read port data into al

    ; A little delay
    nop
    leave               ; Restore stack frame
    ret

; void outb(u16 port, u8 data);
global outb      ; Export outb
outb:
    push ebp
    mov ebp, esp        ; Save stack frame

    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; data
    out dx, al          ; Output al to port number dx

    ; A little delay
    nop
    leave               ; Restore stack frame
    ret

; u16 inw(u16 port);
global inw      ; Export inw
inw:
    push ebp
    mov ebp, esp        ; Save stack frame

    xor eax, eax        ; Clear eax
    mov edx, [ebp + 8]  ; port
    in ax, dx           ; Read port data into al

    ; A little delay
    nop
    leave               ; Restore stack frame
    ret

; void outw(u16 port, u16 data);
global outw      ; Export outw
outw:
    push ebp
    mov ebp, esp        ; Save stack frame

    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; data
    out dx, ax          ; Output al to port number dx

    ; A little delay
    nop
    leave               ; Restore stack frame
    ret
