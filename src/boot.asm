/* Multiboot v1 header + entry point. */
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.global _start
.extern kernel_main

_start:
    cli
    cld
    mov %eax, %esi
    lgdt gdt_descriptor
    ljmp $0x08, $gdt_reload
gdt_reload:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $stack_top, %esp
    mov %esi, %eax
    push %ebx              /* multiboot info pointer */
    push %eax              /* multiboot magic */
    call kernel_main
1:
    hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .rodata
.align 8
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt_end:
gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start
