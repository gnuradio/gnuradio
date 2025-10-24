#!/bin/bash
# Kernel Crypto AES Fuzzing Corpus Generator
CORPUS_DIR="/home/haaken/github-projects/gr-linux-crypto/security/fuzzing/corpus_creation/kernel_crypto_aes_corpus"
mkdir -p "$CORPUS_DIR"

echo "Creating Kernel Crypto AES fuzzing corpus..."

# AES-256 keys (32 bytes)
dd if=/dev/urandom of="$CORPUS_DIR/aes256_key_random" bs=32 count=1 2>/dev/null
printf "\x00%.0s" {1..32} > "$CORPUS_DIR/aes256_key_zeros"
printf "\xFF%.0s" {1..32} > "$CORPUS_DIR/aes256_key_ones"
printf "\x55%.0s" {1..32} > "$CORPUS_DIR/aes256_key_alternating"
printf "\xAA%.0s" {1..32} > "$CORPUS_DIR/aes256_key_alternating2"

# AES-192 keys (24 bytes)
dd if=/dev/urandom of="$CORPUS_DIR/aes192_key_random" bs=24 count=1 2>/dev/null
printf "\x00%.0s" {1..24} > "$CORPUS_DIR/aes192_key_zeros"
printf "\xFF%.0s" {1..24} > "$CORPUS_DIR/aes192_key_ones"

# AES-128 keys (16 bytes)
dd if=/dev/urandom of="$CORPUS_DIR/aes128_key_random" bs=16 count=1 2>/dev/null
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/aes128_key_zeros"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/aes128_key_ones"
printf "\x55%.0s" {1..16} > "$CORPUS_DIR/aes128_key_alternating"

# IVs (16 bytes)
dd if=/dev/urandom of="$CORPUS_DIR/iv_random" bs=16 count=1 2>/dev/null
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/iv_zeros"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/iv_ones"
printf "\x01%.0s" {1..16} > "$CORPUS_DIR/iv_ones_alt"
printf "\x55%.0s" {1..16} > "$CORPUS_DIR/iv_alternating"

# Sequential IVs
printf "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > "$CORPUS_DIR/iv_sequential"
printf "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" > "$CORPUS_DIR/iv_sequential2"

# Plaintext variations
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_random_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_random_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_random_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_random_128" bs=128 count=1 2>/dev/null

# Boundary plaintext sizes
printf "\x00" > "$CORPUS_DIR/plaintext_single_byte"
printf "\x00\x00" > "$CORPUS_DIR/plaintext_two_bytes"
printf "\x00%.0s" {1..15} > "$CORPUS_DIR/plaintext_15_bytes"
printf "\x00%.0s" {1..17} > "$CORPUS_DIR/plaintext_17_bytes"
printf "\x00%.0s" {1..31} > "$CORPUS_DIR/plaintext_31_bytes"
printf "\x00%.0s" {1..33} > "$CORPUS_DIR/plaintext_33_bytes"

# Special plaintext patterns
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/plaintext_zeros_16"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/plaintext_ones_16"
printf "\x55%.0s" {1..16} > "$CORPUS_DIR/plaintext_alternating_16"
printf "\xAA%.0s" {1..16} > "$CORPUS_DIR/plaintext_alternating2_16"

# Sequential plaintext
printf "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > "$CORPUS_DIR/plaintext_sequential_16"
printf "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" > "$CORPUS_DIR/plaintext_sequential2_16"

# Large plaintext
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_large_1k" bs=1024 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_large_4k" bs=4096 count=1 2>/dev/null

# Combined test cases (key + IV + plaintext)
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_random_16" > "$CORPUS_DIR/combined_aes256_random"
cat "$CORPUS_DIR/aes256_key_zeros" "$CORPUS_DIR/iv_zeros" "$CORPUS_DIR/plaintext_zeros_16" > "$CORPUS_DIR/combined_aes256_zeros"
cat "$CORPUS_DIR/aes256_key_ones" "$CORPUS_DIR/iv_ones" "$CORPUS_DIR/plaintext_ones_16" > "$CORPUS_DIR/combined_aes256_ones"

# AES-192 combinations
cat "$CORPUS_DIR/aes192_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_random_16" > "$CORPUS_DIR/combined_aes192_random"
cat "$CORPUS_DIR/aes192_key_zeros" "$CORPUS_DIR/iv_zeros" "$CORPUS_DIR/plaintext_zeros_16" > "$CORPUS_DIR/combined_aes192_zeros"

# AES-128 combinations
cat "$CORPUS_DIR/aes128_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_random_16" > "$CORPUS_DIR/combined_aes128_random"
cat "$CORPUS_DIR/aes128_key_zeros" "$CORPUS_DIR/iv_zeros" "$CORPUS_DIR/plaintext_zeros_16" > "$CORPUS_DIR/combined_aes128_zeros"

# Boundary test cases
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_single_byte" > "$CORPUS_DIR/combined_single_byte"
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_15_bytes" > "$CORPUS_DIR/combined_15_bytes"
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_17_bytes" > "$CORPUS_DIR/combined_17_bytes"

# Large data test cases
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_large_1k" > "$CORPUS_DIR/combined_large_1k"
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_large_4k" > "$CORPUS_DIR/combined_large_4k"

# Corrupted data
dd if=/dev/urandom of="$CORPUS_DIR/corrupted_key" bs=32 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_key" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/corrupted_iv" bs=16 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_iv" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/corrupted_plaintext" bs=16 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_plaintext" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

# Combined corrupted test cases
cat "$CORPUS_DIR/corrupted_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_random_16" > "$CORPUS_DIR/combined_corrupted_key"
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/corrupted_iv" "$CORPUS_DIR/plaintext_random_16" > "$CORPUS_DIR/combined_corrupted_iv"
cat "$CORPUS_DIR/aes256_key_random" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/corrupted_plaintext" > "$CORPUS_DIR/combined_corrupted_plaintext"

echo "Kernel Crypto AES Corpus created: $(ls -1 "$CORPUS_DIR" | wc -l) files"
