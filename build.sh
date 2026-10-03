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
    -g -gdwarf
    
    -Wno-unused-variable
    -Wno-c++17-extensions

    -Iincludes
    -Iprivate-includes

    -Iprivate-includes/UEFI
    -Iprivate-includes/UEFI/AArch64
)

CFLAGS=(
    "${COMMON_FLAGS[@]}"
    --std=c23
    -fno-stack-protector
    -nostdlib
    -MMD
    -MP
)

CPPFLAGS=(
    "${COMMON_FLAGS[@]}"
    -fno-exceptions
    -fno-rtti
    -fshort-wchar
    -MMD
    -MP
    -nostdinc++
    --std=c++26
)

ASFLAGS=(
    -target "$TARGET"
)

LD=(
    ld.lld
    -m arm64pe
    --entry=csl_bootstrap
)


rm -rf build
mkdir build

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

echo "[AR] csl.lib"

OBJS=()

while IFS= read -r -d '' obj; do
    OBJS+=("$obj")
done < <(find build -type f -name '*.o' -print0)

llvm-ar rcs csl.lib "${OBJS[@]}"
