;===============================================================================
; Author        : Lettle
; Create Time   : 2025-10-21
; Copyright     : MIT License
; Description   : The loader of Snailix. It can calculate the memory size and
;                 jump to the protected mode.
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
    jmp ready_to_protected_mode     ; ebx = 0, then jmp to ready_to_protected_mode
.memCheckFail:
    mov si, error
    call print
    hlt
    jmp $

; Prepare to jump to 32-bit protected mode.
ready_to_protected_mode:
    cli                             ; Disable interrupts

    in al, 0x92                     ; Read CMOS byte 0x92
    or al, 0b10                     ; Set bit 1 and bit 2
    out 0x92, al                    ; Write back to CMOS byte 0x92

    lgdt [gdt_ptr]                  ; Load the GDT

    mov eax, cr0                    ; Move the CR0 register to EAX
    or eax, 1                       ; Set the PE bit
    mov cr0, eax                    ; Move the value back to CR0
    
    jmp dword code_selector:protected_mode ; Far jump to the protected mode

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

[bits 32]
protected_mode:
    ; Set up the segment registers
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Print '32-bit' to the left of the first line.
    mov [0xb8094], byte '3'
    mov [0xb8096], byte '2'
    mov [0xb8098], byte '-'
    mov [0xb809a], byte 'b'
    mov [0xb809c], byte 'i'
    mov [0xb809e], byte 't'

    ; Set up the stack
    mov esp, 0x10000

    ; Load the kernel
    mov edi, 0x10000                ; Destination address for the kernel
    mov ecx, 10                     ; Starting sector number
    mov bl , 200                    ; Number of sectors to read
    call read_disk_32

    ; Param1: eax = Total size of the memory, edx = High 8 bit of the total size
    call calcMemSize_32
    ; Param2: ecx = Magic number of the kernel
    mov ecx, 0x22CAFE33

    jmp dword code_selector:0x10000

    ; If this code is reached, an error occurred.
    ud2

; This is a procedural function and does not need to save registers.
calcMemSize_32:
    ; Calculate total memory size, save it to eax
    ; Using: memCheckBuf, ARDSCount  -->  eax (Memory Size) -->  edx (High 8 bit, if needed)
    xor eax, eax                    ; Use eax to store the total memory size.
    xor ebx, ebx                    ; Use ebx as a counter.
    
    mov esi, memCheckBuf            ; ds:esi -> Buffer
    mov ecx, [ARDSCount]            ; ecx = Number of ARDS
    .calcLoop:
        cmp dword [esi + 16], 1     ; Check if the type is usable memory
        jne .skip                   ; If not, skip to the next ARDS

        ; Calculate the size of the memory region
        mov edx, [esi + 12]         ; edx = high 32 bits
        mov ebx, [esi + 8]          ; ebx = low 32 bits

        ; Add the 64 bits to the total memory size
        add eax, ebx                ; Add the low 32 bits
        adc edx, 0

    .skip:
        add esi, 20                 ; Move to the next ARDS
        loop .calcLoop              ; Loop until all ARDS have been checked

    ; Now, eax = LOW  8 BIT of the total memory size in bytes.
    ;      edx = HIGH 8 BIT of the total memory size in bytes.
    ; If you want to convert it to kilobytes, right shift by 10 bits.
    ; If you want to convert it to megabytes, right shift by 20 bits.
    ret

read_disk_32:
    ; Sets the number of sectors
    mov dx, 0x1f2
    mov al, bl
    out dx, al
    ; The first eight bits of the starting sector
    inc dx; 0x1f3
    mov al, cl      
    out dx, al
    ; The middle eight bits of the starting sector
    inc dx; 0x1f4
    shr ecx, 8
    mov al, cl      
    out dx, al
    ; The high eight bits of the starting sector
    inc dx; 0x1f5
    shr ecx, 8
    mov al, cl      
    out dx, al
    ; Set the high four bits to 0
    inc dx; 0x1f6
    shr ecx, 8
    and cl, 0b1111  
    ; Master disk - LBA mode
    mov al, 0b1110_0000;
    or al, cl
    out dx, al
    ; Read disk
    inc dx; 0x1f7
    mov al, 0x20
    out dx, al
    ; Get the number of sectors read
    xor ecx, ecx
    mov cl, bl

    .read:
        push cx; 保存 cx
        call .waits; 等待数据准备完毕
        call .reads; 读取一个扇区
        pop cx; 恢复 cx
        loop .read
    ret

    .waits:
        mov dx, 0x1f7
        .check:
            in al, dx
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check
        ret

    .reads:
        mov dx, 0x1f0
        mov cx, 256; 一个扇区 256 字
        .readw:
            in ax, dx
            mov [edi], ax
            add edi, 2
            loop .readw
        ret


; ==================================================
; GDT
; ==================================================
memory_base equ 0; 内存开始的位置：基地址

; 内存界限 4G / 4K - 1
memory_limit equ ((1024 * 1024 * 1024 * 4) / (1024 * 4)) - 1

gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base
gdt_base:
    dd 0, 0; NULL 描述符
gdt_code:
    dw memory_limit & 0xffff; 段界限 0 ~ 15 位
    dw memory_base & 0xffff; 基地址 0 ~ 15 位
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位
    ; 存在 - dlp 0 - S _ 代码 - 非依从 - 可读 - 没有被访问过
    db 0b_1_00_1_1_0_1_0;
    ; 4k - 32 位 - 不是 64 位 - 段界限 16 ~ 19
    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf;
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位
gdt_data:
    dw memory_limit & 0xffff; 段界限 0 ~ 15 位
    dw memory_base & 0xffff; 基地址 0 ~ 15 位
    db (memory_base >> 16) & 0xff; 基地址 16 ~ 23 位
    ; 存在 - dlp 0 - S _ 数据 - 向上 - 可写 - 没有被访问过
    db 0b_1_00_1_0_0_1_0;
    ; 4k - 32 位 - 不是 64 位 - 段界限 16 ~ 19
    db 0b1_1_0_0_0000 | (memory_limit >> 16) & 0xf;
    db (memory_base >> 24) & 0xff; 基地址 24 ~ 31 位
gdt_end:

code_selector equ (1 << 3)
data_selector equ (2 << 3)

; Store the ARDS structure
; 256 bytes are for the alignment of 32 bits.
; 256/20=12.8, so this buffer can hold 12 ARDS.
memCheckBuf:    times 256 db 0
ARDSCount:       dd 0
