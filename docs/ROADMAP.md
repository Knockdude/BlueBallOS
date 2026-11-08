# BlueBallOS roadmap

The milestones below prioritize a trustworthy kernel before higher-level
features. A checked box means the capability exists in some form; it does not
necessarily mean production readiness.

## 1. Boot and diagnostics
- [x] Multiboot v1 entry and bootstrap stack
- [x] Private flat GDT loaded before entering C
- [x] VGA text output and COM1 boot diagnostics
- [x] Basic interactive kernel monitor
- [ ] Parse Multiboot information and memory map
- [ ] Add linker/build assertions for kernel layout

## 2. CPU and interrupts
- [x] IDT exception gates and panic halt
- [x] Legacy PIC remap and timer/keyboard IRQs
- [ ] TSS and double-fault task gate
- [ ] APIC timer and multiprocessor startup
- [ ] Structured kernel logging and panic register dump

## 3. Memory
- [x] 4 MiB identity-mapped paging bootstrap
- [x] Fixed 1 MiB bump allocator
- [ ] Physical frame allocator from firmware memory map
- [ ] Dynamic page tables and page-fault handling
- [ ] Reclaiming heap allocator and memory diagnostics

## 4. Processes and userland
- [ ] Kernel context switching and preemptive scheduling
- [ ] Ring-3 process creation and ELF loader
- [ ] System-call ABI
- [ ] IPC and process lifecycle
- [ ] Minimal libc and user-space shell

## 5. Devices and storage
- [x] Basic PS/2 set-1 keyboard input
- [ ] PCI device discovery and driver interfaces
- [ ] PS/2 mouse packet decoding
- [ ] Reliable block-device abstraction and timeouts
- [ ] VFS and RAM filesystem
- [ ] Tested persistent filesystem
- [ ] USB, AHCI/NVMe and real NIC drivers

## 6. Desktop and applications
- [ ] Framebuffer negotiation and graphics primitives
- [ ] Input/event abstraction and compositor
- [ ] Window manager and desktop shell
- [ ] Sandboxed user applications
- [ ] Package format, signatures and update mechanism

## 7. Networking and browser
- [ ] Ethernet driver, ARP, IPv4 and ICMP
- [ ] UDP, TCP, DHCP and DNS
- [ ] TLS and HTTP client
- [ ] HTML/CSS layout, fonts, image codecs and browser sandbox
- [ ] JavaScript engine (separate project/dependency)

## Release criteria

A daily-use OS needs tested hardware support, reliable storage, isolated
user-space processes, a security model, recovery/update mechanisms, and a
maintainable application ecosystem. The current kernel monitor is an early
bring-up milestone, not a desktop release.
