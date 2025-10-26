;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-21
; Copyright     : MIT License
; Description   : The loader of Snailix. It can calculate the memory size.
;===============================================================================
; Note:         : The code is written in NASM syntax.
;===============================================================================
; The code will be loaded at 0x1000
[ORG 0x1000]
dw 0x2233       ; Magic number, used to determine whether an error has occurred

_loader:
    ; Print a string to the screen
    mov si, loading
    call print

    ; Check memory size
    mov ebx, 0
    mov di,  memCheckBuf
.memCheckLoop:
    ; Using: memCheckBuf (Result), ARDSCount (Result)
    mov eax, 0E820h                 ; eax = 0000E820h
    mov ecx, 20                     ; ecx = size of ARDS
    mov edx, 0534D4150h             ; edx = 'SMAP'
    int 15h                         ; BIOS interrupt
    jc .memCheckFail                ; If carry flag is set, an error occurred
    add di, 20
    inc dword [ARDSCount]            ; Increment the ARDS count
    cmp ebx, 0
    jne .memCheckLoop               ; If ebx is not 0, there are more ARDS to check
    jmp .finish                     ; ebx = 0, then jmp to finish
.memCheckFail:
    mov si, error
    call print
    hlt
    jmp $

.finish:
    jmp $


; Print a string to the screen
; Params:
;   es:di - pointer to the string
print:
    mov ah, 0x0e
.next:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

error:
    mov si, .error_msg
    call print
    hlt
    jmp $
    .error_msg:
        db "[16-bit Loader] Error occurred!", 10, 13, 0

loading:
    db "[16-bit Loader] Loading Snailix...", 10, 13, 0

; Store the ARDS structure
; 256 bytes are for the alignment of 32 bits.
; 256/20=12.8, so this buffer can hold 12 ARDS.
memCheckBuf:    times 256 db 0
ARDSCount:       dd 0
