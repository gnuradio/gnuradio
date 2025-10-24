#!/bin/bash
# OpenSSL Wrapper Fuzzing Corpus Generator
CORPUS_DIR="/home/haaken/github-projects/gr-linux-crypto/security/fuzzing/corpus_creation/openssl_corpus"
mkdir -p "$CORPUS_DIR"

echo "Creating OpenSSL wrapper fuzzing corpus..."

# AES keys (various sizes)
dd if=/dev/urandom of="$CORPUS_DIR/aes128_key" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/aes192_key" bs=24 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/aes256_key" bs=32 count=1 2>/dev/null

# Weak keys
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/aes128_key_zeros"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/aes128_key_ones"
printf "\x00%.0s" {1..32} > "$CORPUS_DIR/aes256_key_zeros"
printf "\xFF%.0s" {1..32} > "$CORPUS_DIR/aes256_key_ones"

# IVs
dd if=/dev/urandom of="$CORPUS_DIR/iv_random" bs=16 count=1 2>/dev/null
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/iv_zeros"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/iv_ones"

# Plaintext variations
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/plaintext_128" bs=128 count=1 2>/dev/null

# Hash data
dd if=/dev/urandom of="$CORPUS_DIR/hash_data_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hash_data_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hash_data_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hash_data_128" bs=128 count=1 2>/dev/null

# HMAC keys and data
dd if=/dev/urandom of="$CORPUS_DIR/hmac_key_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hmac_key_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hmac_key_64" bs=64 count=1 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/hmac_data_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hmac_data_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/hmac_data_64" bs=64 count=1 2>/dev/null

# RSA test data
dd if=/dev/urandom of="$CORPUS_DIR/rsa_data_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/rsa_data_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/rsa_data_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/rsa_data_128" bs=128 count=1 2>/dev/null

# Boundary cases
printf "\x00" > "$CORPUS_DIR/single_byte"
printf "\x00\x00" > "$CORPUS_DIR/two_bytes"
printf "\x00%.0s" {1..15} > "$CORPUS_DIR/15_bytes"
printf "\x00%.0s" {1..17} > "$CORPUS_DIR/17_bytes"

# Special patterns
printf "\x55%.0s" {1..16} > "$CORPUS_DIR/pattern_55_16"
printf "\xAA%.0s" {1..16} > "$CORPUS_DIR/pattern_AA_16"
printf "\x33%.0s" {1..16} > "$CORPUS_DIR/pattern_33_16"
printf "\xCC%.0s" {1..16} > "$CORPUS_DIR/pattern_CC_16"

# Sequential data
printf "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > "$CORPUS_DIR/sequential_16"
printf "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" > "$CORPUS_DIR/sequential2_16"

# Large data
dd if=/dev/urandom of="$CORPUS_DIR/large_1k" bs=1024 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/large_4k" bs=4096 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/large_16k" bs=16384 count=1 2>/dev/null

# Combined AES test cases
cat "$CORPUS_DIR/aes128_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_16" > "$CORPUS_DIR/aes128_cbc_test"
cat "$CORPUS_DIR/aes192_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_32" > "$CORPUS_DIR/aes192_cbc_test"
cat "$CORPUS_DIR/aes256_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_64" > "$CORPUS_DIR/aes256_cbc_test"

# Combined HMAC test cases
cat "$CORPUS_DIR/hmac_key_16" "$CORPUS_DIR/hmac_data_16" > "$CORPUS_DIR/hmac_sha1_test"
cat "$CORPUS_DIR/hmac_key_32" "$CORPUS_DIR/hmac_data_32" > "$CORPUS_DIR/hmac_sha256_test"
cat "$CORPUS_DIR/hmac_key_64" "$CORPUS_DIR/hmac_data_64" > "$CORPUS_DIR/hmac_sha512_test"

# Combined hash test cases
cp "$CORPUS_DIR/hash_data_16" "$CORPUS_DIR/sha1_test"
cp "$CORPUS_DIR/hash_data_32" "$CORPUS_DIR/sha256_test"
cp "$CORPUS_DIR/hash_data_64" "$CORPUS_DIR/sha512_test"

# Combined RSA test cases
cp "$CORPUS_DIR/rsa_data_16" "$CORPUS_DIR/rsa_encrypt_test"
cp "$CORPUS_DIR/rsa_data_32" "$CORPUS_DIR/rsa_sign_test"
cp "$CORPUS_DIR/rsa_data_64" "$CORPUS_DIR/rsa_verify_test"

# Corrupted data
dd if=/dev/urandom of="$CORPUS_DIR/corrupted_key" bs=32 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_key" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/corrupted_iv" bs=16 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_iv" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/corrupted_data" bs=32 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_data" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

# Combined corrupted test cases
cat "$CORPUS_DIR/corrupted_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/plaintext_16" > "$CORPUS_DIR/corrupted_key_test"
cat "$CORPUS_DIR/aes256_key" "$CORPUS_DIR/corrupted_iv" "$CORPUS_DIR/plaintext_16" > "$CORPUS_DIR/corrupted_iv_test"
cat "$CORPUS_DIR/aes256_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/corrupted_data" > "$CORPUS_DIR/corrupted_data_test"

# Edge case combinations
cat "$CORPUS_DIR/aes256_key_zeros" "$CORPUS_DIR/iv_zeros" "$CORPUS_DIR/single_byte" > "$CORPUS_DIR/edge_case_zeros"
cat "$CORPUS_DIR/aes256_key_ones" "$CORPUS_DIR/iv_ones" "$CORPUS_DIR/pattern_55_16" > "$CORPUS_DIR/edge_case_ones"
cat "$CORPUS_DIR/aes256_key" "$CORPUS_DIR/iv_random" "$CORPUS_DIR/large_1k" > "$CORPUS_DIR/edge_case_large"

echo "OpenSSL Corpus created: $(ls -1 "$CORPUS_DIR" | wc -l) files"
