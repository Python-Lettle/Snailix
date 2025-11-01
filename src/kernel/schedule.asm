;===============================================================================
; Author        : Lettle
; Create Time   : 2025-11-1
; Copyright     : MIT License
; Description   : Task Switch Assembly
;===============================================================================
[bits 32]

section .text

global task_switch
task_switch:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    mov eax, esp;
    and eax, 0xfffff000     ; current
    
    mov [eax], esp

    mov eax, [ebp + 8]      ; next
    mov esp, [eax]

    pop edi
    pop esi
    pop ebx

    pop ebp
    ret