#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
OUT="$ROOT/build"
mkdir -p "$OUT"

CC="${CC:-gcc}"
AS="${AS:-as}"
LD="${LD:-ld}"

CFLAGS="-m32 -mno-sse -mno-mmx -mpreferred-stack-boundary=2 -mincoming-stack-boundary=2 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -nostdlib -Wall -Wextra -O2 -Isrc"
ASFLAGS="--32"

# Check for a usable 32-bit compiler.
if ! echo 'int x;' | "$CC" -m32 -ffreestanding -x c -c -o "$OUT/test.o" - >/dev/null 2>&1; then
  echo "A 32-bit GCC toolchain is required. On Debian/Ubuntu/WSL try:"
  echo "  sudo apt update && sudo apt install build-essential gcc-multilib binutils grub-pc-bin xorriso qemu-system-x86"
  exit 1
fi
rm -f "$OUT/test.o"

rm -rf "$OUT/obj" "$OUT/isodir"
mkdir -p "$OUT/obj" "$OUT/isodir/boot/grub"

mapfile -t CFILES < <(find src -name '*.c' -print | sort)
mapfile -t ASFILES < <(find src -name '*.asm' -print | sort)
OBJS=()

for src in "${CFILES[@]}"; do
  name="$(echo "$src" | sed 's#^src/##; s#[/.-]#_#g')_c"
  obj="$OUT/obj/${name}.o"
  echo "CC $src"
  "$CC" $CFLAGS -c "$src" -o "$obj"
  OBJS+=("$obj")
done

for src in "${ASFILES[@]}"; do
  name="$(echo "$src" | sed 's#^src/##; s#[/.-]#_#g')_asm"
  obj="$OUT/obj/${name}.o"
  echo "AS $src"
  "$AS" $ASFLAGS "$src" -o "$obj"
  OBJS+=("$obj")
done

"$LD" -m elf_i386 -T src/linker.ld -o "$OUT/kernel.elf" "${OBJS[@]}"

if command -v grub-file >/dev/null 2>&1; then
  grub-file --is-x86-multiboot "$OUT/kernel.elf" || {
    echo "The kernel is not a valid Multiboot v1 image."
    exit 1
  }
fi

cp "$OUT/kernel.elf" "$OUT/isodir/boot/blueballos.kernel"
cp boot/grub.cfg "$OUT/isodir/boot/grub/grub.cfg"

if ! command -v grub-mkrescue >/dev/null 2>&1; then
  echo "grub-mkrescue is required. Install grub-pc-bin and xorriso."
  exit 1
fi

grub-mkrescue -o "$OUT/blueballos-ultimate.iso" "$OUT/isodir" >/dev/null

echo
echo "======================================"
echo " BlueBallOS Ultimate ISO READY"
echo " $OUT/blueballos-ultimate.iso"
echo "======================================"
