#!/bin/bash

# LibFuzzer build script for gr-linux-crypto
# This script compiles LibFuzzer-compatible fuzzing harnesses

set -e

echo "=== LibFuzzer Build Script ==="
echo "Building LibFuzzer-compatible fuzzing harnesses..."

# Create build directory
BUILD_DIR="libfuzzer_build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Set compiler flags for LibFuzzer
export CXXFLAGS="-fsanitize=fuzzer,address,undefined -g -O1 -fno-omit-frame-pointer"
export LDFLAGS="-fsanitize=fuzzer,address,undefined"

echo "Compiler flags: $CXXFLAGS"
echo "Linker flags: $LDFLAGS"

# Build kernel keyring fuzzer
echo "Building kernel_keyring_libfuzzer..."
clang++ $CXXFLAGS -o kernel_keyring_libfuzzer ../kernel_keyring_libfuzzer.cpp $LDFLAGS

# Build kernel crypto AES fuzzer
echo "Building kernel_crypto_aes_libfuzzer..."
clang++ $CXXFLAGS -o kernel_crypto_aes_libfuzzer ../kernel_crypto_aes_libfuzzer.cpp $LDFLAGS

# Build nitrokey fuzzer
echo "Building nitrokey_libfuzzer..."
clang++ $CXXFLAGS -o nitrokey_libfuzzer ../nitrokey_libfuzzer.cpp $LDFLAGS

# Build OpenSSL fuzzer
echo "Building openssl_libfuzzer..."
clang++ $CXXFLAGS -o openssl_libfuzzer ../openssl_libfuzzer.cpp $LDFLAGS

echo ""
echo "=== Build Complete ==="
echo "LibFuzzer binaries created:"
ls -la *.libfuzzer 2>/dev/null || echo "No .libfuzzer files found"
ls -la kernel_* nitrokey_* openssl_* 2>/dev/null || echo "No fuzzer binaries found"

echo ""
echo "=== Fuzzer Information ==="
for fuzzer in kernel_* nitrokey_* openssl_*; do
    if [ -f "$fuzzer" ]; then
        echo "--- $fuzzer ---"
        echo "Size: $(stat -c%s "$fuzzer") bytes"
        echo "Type: $(file "$fuzzer")"
        echo ""
    fi
done

echo "=== LibFuzzer Build Complete ==="
echo "Ready to run LibFuzzer tests!"
