.section .text
.extern irq_dispatch
.extern exception_dispatch

.macro ISR_NOERR number
.global isr\number
isr\number:
    pushl $0
    pushl $\number
    jmp exception_common
.endm

.macro ISR_ERR number
.global isr\number
isr\number:
    pushl $\number
    jmp exception_common
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR   29
ISR_ERR   30
ISR_NOERR 31

.global isr_default
isr_default:
    pushl $0
    pushl $255
    jmp exception_common

exception_common:
    pusha
    cld
    movl 32(%esp), %eax
    pushl %eax
    call exception_dispatch
1:
    cli
    hlt
    jmp 1b

.global irq0_stub
irq0_stub:
    pusha
    cld
    pushl $32
    call irq_dispatch
    addl $4, %esp
    popa
    iret

.global irq1_stub
irq1_stub:
    pusha
    cld
    pushl $33
    call irq_dispatch
    addl $4, %esp
    popa
    iret
