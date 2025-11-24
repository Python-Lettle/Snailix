;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-21
; Copyright     : MIT License
; Description   : The boot sector code of Snailix
;===============================================================================
; Note          : 
;   The code is written in NASM syntax.
;   When I was writing this program, I aimed for ultimate simplicity.
;   By default, no errors occur in the boot sector,
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

    ; Read the loader program to 0x1000
    mov edi, 0x1000
    mov ecx, 2
    mov bl , 4
    call read_disk

    ; Check if the loader program is valid
    cmp word [0x1000], 0x2233
    jnz error

    ; Jump to the loader program,
    ; Don't forget the 2 byte magic number at 0x1000 
    jmp 0:0x1002

    ; Block machine here, but that's never going to happen
    jmp $

; Read the data through the ATA PIO mode using the LBA28 addressing mode
; Params:
;   edi: target address
;   ecx: starting sector
;   bl : number of sectors
read_disk:
    ; Sets the number of read and write sectors
    mov dx, 0x1F2       ; Sector number port
    mov al, bl          ; The number of sectors
    out dx, al

    inc dx              ; dx = 0x1F3
    mov al, cl          ; The first 8 bits of the starting sector
    out dx, al

    inc dx              ; dx = 0x1F4
    shr ecx, 8
    mov al, cl          ; The middle 8 bits of the starting sector

    inc dx              ; dx = 0x1F5
    shr ecx, 8
    mov al, cl          ; The high 8 bits of the starting sector
    out dx, al

    inc dx              ; dx = 0x1F6
    shr ecx, 8
    and cl, 0b1111      ; Reset the high 4 digits to zero

    mov al, 0b1110_0000
    or al, cl
    out dx, al          ; Select the main disk and LBA addressing

    inc dx              ; dx = 0x1F7
    mov al, 0x20        ; Read disk
    out dx, al

    xor ecx, ecx
    mov cl, bl          ; Get the number of read and write sectors

    .read:
        push cx         ; Save cx
        call .waits     ; Wait for the data to be ready
        call .reads     ; Read data
        pop cx          ; Restore cx
        loop .read
    ret

    .waits:
        mov dx, 0x1F7
        .check:
            in al, dx
            and al,  0b1000_1000
            cmp al,  0b0000_1000
            jnz .check
        ret
    
    .reads:
        mov dx, 0x1F0
        mov cx, 256     ; One sector contains 256 words
        .readw:
            in ax, dx
            mov [edi], ax
            add edi, 2
            loop .readw
        ret

error:
    ; Write the error messages to video memory
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