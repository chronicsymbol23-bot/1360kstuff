BITS 64
SECTION .text
global _start

_start:
    ; 1. Define your exact 13.60 kernel filesystem verification target address
    ; Since we are building a flat bare-metal injection file, the network loader 
    ; will pass the live kernel base pointer inside the RSI or RDI registers.
    ; For an absolute hook address, we load the offset directly:
    mov rbx, 0x001e0e0a7

    ; 2. Temporarily lift hardware CPU Write Protection by flipping the CR0 register WP bit
    mov rax, cr0
    mov rdx, rax                  ; Save original CR0 state into RDX
    and rax, 0xfffffffffffeffff   ; Clear the 16th (Write Protect) bit
    mov cr0, rax                  ; Apply change -> Kernel memory becomes writable!

    ; 3. Overwrite the signature loop with success instructions: XOR EAX, EAX; RET (\x31\xC0\xC3)
    mov byte [rbx], 0x31
    mov byte [rbx+1], 0xC0
    mov byte [rbx+2], 0xC3

    ; 4. Instantly restore the original CR0 tracking status to protect system stability
    mov cr0, rdx
    ret
