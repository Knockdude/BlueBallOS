# BlueBallOS

BlueBallOS is an experimental 32-bit x86 operating-system kernel project. It boots
through GRUB using Multiboot v1 and installs its own flat GDT before entering C.
It currently provides a protected-mode kernel,
identity-mapped paging, a fixed-size kernel heap, exception reporting, timer and
PS/2 keyboard interrupts, VGA/serial diagnostics, and a small interactive kernel
monitor.

This is an educational foundation, not a general-purpose desktop operating
system. It does not yet provide user-mode processes, a real scheduler, a
filesystem, networking, a graphical desktop, or a browser. The directories for
several of those features contain prototypes or placeholders only.

## Build

Build on Debian or Ubuntu (including WSL with a Linux distribution installed):

```sh
sudo apt update
sudo apt install build-essential gcc-multilib binutils grub-pc-bin xorriso qemu-system-x86
./build.sh
```

The bootable ISO is written to `build/blueballos-ultimate.iso`. The build script
also checks the Multiboot header when `grub-file` is available.

Run the ISO in QEMU:

```sh
qemu-system-i386 -cdrom build/blueballos-ultimate.iso -m 256
```

The kernel monitor accepts `help`, `about`, `uptime`, and `clear`. In QEMU, use
the virtual keyboard to enter commands. Boot diagnostics are also sent to COM1
so automated tests can verify that the kernel reached the monitor.

## Project layout

- `src/boot.asm`: Multiboot header and kernel entry.
- `src/interrupts/`: IDT, PIC setup, CPU exception stubs, and IRQ entry points.
- `src/drivers/`: timer, keyboard, storage, mouse, and audio experiments.
- `src/memory/`: early paging and heap implementations.
- `src/`: additional exploratory GUI, filesystem, networking, package, and app
  modules; consult [the architecture notes](docs/ARCHITECTURE.md) for their
  current status.
- `.github/workflows/build.yml`: build, QEMU boot smoke test, and ISO artifact.

## Current limitations

- Paging maps only the first 4 MiB and has no dynamic page-table allocator.
- The heap is a 1 MiB bump allocator; `kfree` does not reclaim memory.
- The timer interrupt counts ticks but does not perform task switching.
- The keyboard driver supports basic set-1 scan codes; there is no USB input.
- The Multiboot information structure is not yet parsed for available memory
  or a framebuffer.
- No ring 3, system-call ABI, virtual filesystem, persistent storage integration,
  or real network interface driver is implemented.
