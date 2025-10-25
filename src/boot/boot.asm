;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-21
; Copyright     : MIT License
; Description   : The boot sector code of Snailix
;===============================================================================
; Note          : 
;   The code is written in NASM syntax.
;   When I was writing this program, I aimed for ultimate simplicity.
;   By default, no errors will occur in the boot sector,
; so I didn't write the 'print' function here to output anything.
;===============================================================================
; The code will be loaded at 0x7c00 and executed by the BIOS.
[ORG 0x7C00]

_boot:
    ; Set the screen to TEXT MODE and clear the screen
    mov ax, 3
    int 0x10

    ; Initialize the segment register
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Test the error function by jumping to itß
    jmp error

    ; Block machine here, but that's never going to happen
    jmp $

error:
    ; Write the error messages to video memory directly.
    mov ax, 0xB800
    mov es, ax
    mov [es:0x0000], byte 'E'
    mov [es:0x0002], byte 'R'
    mov [es:0x0004], byte 'R'
    mov [es:0x0006], byte 'O'
    mov [es:0x0008], byte 'R'
    mov [es:0x000A], byte '!'
    hlt
    jmp $

; End of the boot sector
times 510 - ($ - $$) db 0   ; Padding to 510 bytes
db 0x55, 0xAA               ; Boot sector flags