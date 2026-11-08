# BlueBallOS architecture and implementation status

## Boot path

```text
GRUB (Multiboot v1)
  -> assembly entry, private flat GDT, and 16 KiB bootstrap stack
  -> 32-bit freestanding kernel
  -> identity-map first 4 MiB and enable paging
  -> initialize fixed 1 MiB heap
  -> load IDT, remap/mask PIC, initialize PS/2 keyboard and PIT
  -> enable interrupts and run the VGA/COM1 kernel monitor
```

CPU exceptions are routed to a panic handler. IRQ0 increments a 100 Hz tick
counter, and IRQ1 translates basic set-1 keyboard make codes into a bounded
single-producer/single-consumer character queue.

## Layer status

| Layer | Status |
|---|---|
| Boot | GRUB Multiboot v1 entry and private flat GDT; boot info is not parsed |
| Kernel | 32-bit ring-0 kernel monitor, VGA text output, COM1 diagnostics |
| Interrupts | IDT gates for exceptions and timer/keyboard IRQs; legacy PIC |
| Virtual memory | Bootstrap identity map for the first 4 MiB only |
| Heap | Fixed 1 MiB aligned bump allocator; no reclamation |
| Scheduling | Task table prototype; no context switching or runnable-task dispatch |
| Keyboard | Basic PS/2 set-1 scan-code translation |
| Timer | PIT IRQ tick counter; no preemption |
| Mouse | Placeholder; no PS/2 packet handling |
| Disk / FAT | ATA and FAT16 prototypes; no mounted VFS or integrated boot storage |
| GUI / GPU | Experimental code; no framebuffer negotiation or desktop compositor |
| Network / browser | API placeholders; no NIC, TCP/IP, TLS, or rendering engine |
| Packages / apps / games | API prototypes; no executable loading or user mode |

## Near-term architecture goals

1. Parse Multiboot memory maps and reserve kernel/bootloader-owned regions.
2. Replace the single identity map with physical-frame and virtual-region
   allocators, including guardrails for unaligned and out-of-range mappings.
3. Add GDT/TSS setup, ring-3 entry, a syscall ABI, and real task context switching.
4. Introduce a VFS with a tested RAM filesystem before adding persistent disk
   filesystems.
5. Add hardware discovery and drivers behind explicit device interfaces.
6. Only after user mode and storage are stable, build a shell, graphics stack,
   networking, and applications as separately testable components.

The current kernel should be treated as an early bring-up target. The exploratory
modules are not yet integrated services, despite their source directories.
