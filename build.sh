#!/usr/bin/env bash
set -euo pipefail

CC="clang"
CXX="clang++"
AS="clang"

TARGET="aarch64-none-elf"

CFLAGS=(
    -target "$TARGET"
    -ffreestanding
    -fno-stack-protector
    -Wall
    -Wextra
    -Isrc
    -Iincludes/
    -MMD -MP
)

CPPFLAGS=(
    "${CFLAGS[@]}"
    -fno-exceptions
    -fno-rtti
)

ASFLAGS=(
    -target "$TARGET"
)

LD=(
    clang
    -target aarch64-none-elf
    -fuse-ld=lld
    -nostdlib
    -Tsrc/linker.ld
)

rm -r build

mkdir -p build

find src -type f | while read -r file; do
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

echo "[LD] csl.elf"

OBJS=()

while IFS= read -r -d '' obj; do
    OBJS+=("$obj")
done < <(find build -type f -name '*.o' -print0)

"${LD[@]}" "${OBJS[@]}" -o csl.elf