#!/usr/bin/env bash
# build.sh

set -euo pipefail

CC="clang"
CXX="clang++"
AS="clang"

TARGET="aarch64-unknown-windows"
SRC_DIR="src"

pkill qemu-system-aar || true

COMMON_FLAGS=(
    --target=$TARGET
    -ffreestanding
    -Wall
    -Wextra
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Werror

    -Wno-unused-variable
    -Wno-c++17-extensions

    -Iincludes
    -Iincludes/CSL
    -Iincludes/uefi-headers
    -Iincludes/uefi-headers/AArch64
)

CFLAGS=(
    "${COMMON_FLAGS[@]}"
    -fno-stack-protector
    -MMD
    -MP
)

CPPFLAGS=(
    "${COMMON_FLAGS[@]}"
    -fno-exceptions
    -fno-rtti
    -MMD
    -MP
)

ASFLAGS=(
    -target "$TARGET"
)

LD=(
    clang
    -target $TARGET
    -fuse-ld=lld-link
    -nostdlib
    -Wl,/entry:csl_bootstrap
    -Wl,/subsystem:efi_application
    -o BOOTAA64.EFI
)

rm -r build

mkdir -p build

find $SRC_DIR -type f | while read -r file; do
    rel="${file#src/}"
    obj="build/${rel%.*}.o"

    mkdir -p "$(dirname "$obj")"

    case "$file" in
        *.c)
            echo "[CC] $file"
            "$CC" "${CFLAGS[@]}" -c "$file" -o "$obj"
            ;;
        *.cpp)
            echo "[CXX] $file"
            "$CXX" "${CPPFLAGS[@]}" -c "$file" -o "$obj"
            ;;
        *.s|*.S)
            echo "[AS] $file"
            "$AS" "${ASFLAGS[@]}" -c "$file" -o "$obj"
            ;;
    esac
done

echo "[LD] csl.efi"

OBJS=()

while IFS= read -r -d '' obj; do
    OBJS+=("$obj")
done < <(find build -type f -name '*.o' -print0)

"${LD[@]}" "${OBJS[@]}" -o csl.efi

cp -v csl.efi esp/EFI/BOOT/BOOTAA64.efi

llvm-readobj --coff-basereloc csl.efi
