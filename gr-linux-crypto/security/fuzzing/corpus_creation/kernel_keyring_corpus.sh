#!/bin/bash
# Kernel Keyring Fuzzing Corpus Generator
CORPUS_DIR="/home/haaken/github-projects/gr-linux-crypto/security/fuzzing/corpus_creation/kernel_keyring_corpus"
mkdir -p "$CORPUS_DIR"

echo "Creating Kernel Keyring fuzzing corpus..."

# Valid key types and descriptions
echo "user" > "$CORPUS_DIR/key_type_user"
echo "logon" > "$CORPUS_DIR/key_type_logon"
echo "keyring" > "$CORPUS_DIR/key_type_keyring"
echo "big_key" > "$CORPUS_DIR/key_type_big_key"

# Valid key descriptions
echo "gr_crypto_test_key" > "$CORPUS_DIR/desc_simple"
echo "test_key_123" > "$CORPUS_DIR/desc_numeric"
echo "my_secret_key" > "$CORPUS_DIR/desc_secret"
echo "key_with_underscores" > "$CORPUS_DIR/desc_underscores"
echo "key-with-dashes" > "$CORPUS_DIR/desc_dashes"
echo "key.with.dots" > "$CORPUS_DIR/desc_dots"

# Key data variations
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_128" bs=128 count=1 2>/dev/null

# Weak keys
printf "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" > "$CORPUS_DIR/key_data_zeros_16"
printf "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" > "$CORPUS_DIR/key_data_ones_16"
printf "\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55" > "$CORPUS_DIR/key_data_alternating_16"
printf "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA" > "$CORPUS_DIR/key_data_alternating2_16"

# Sequential keys
printf "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > "$CORPUS_DIR/key_data_sequential_16"
printf "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" > "$CORPUS_DIR/key_data_sequential2_16"

# Boundary sizes
printf "\x00" > "$CORPUS_DIR/key_data_single_byte"
printf "\x00\x00" > "$CORPUS_DIR/key_data_two_bytes"
dd if=/dev/urandom of="$CORPUS_DIR/key_data_max" bs=1024 count=1 2>/dev/null

# Special characters in descriptions
echo "key_with_special_chars_!@#$%^&*()" > "$CORPUS_DIR/desc_special_chars"
echo "key_with_unicode_测试" > "$CORPUS_DIR/desc_unicode"
echo "key_with_spaces and more" > "$CORPUS_DIR/desc_spaces"
echo "key_with_tabs	and	more" > "$CORPUS_DIR/desc_tabs"

# Empty and null descriptions
echo "" > "$CORPUS_DIR/desc_empty"
printf "\x00" > "$CORPUS_DIR/desc_null"
printf "\x00\x00\x00\x00" > "$CORPUS_DIR/desc_multiple_nulls"

# Long descriptions
printf "a%.0s" {1..255} > "$CORPUS_DIR/desc_long_255"
printf "a%.0s" {1..1000} > "$CORPUS_DIR/desc_very_long"

# Key data with special patterns
printf "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10" > "$CORPUS_DIR/key_data_hex_pattern"
printf "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE" > "$CORPUS_DIR/key_data_deadbeef"

# Corrupted data
dd if=/dev/urandom of="$CORPUS_DIR/key_data_corrupted" bs=32 count=1 2>/dev/null
# Flip some bits
dd if="$CORPUS_DIR/key_data_corrupted" of="$CORPUS_DIR/key_data_corrupted_flipped" bs=1 count=32 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/key_data_corrupted_flipped" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

# Combined test cases (type + description + data)
cat "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_simple" "$CORPUS_DIR/key_data_random_32" > "$CORPUS_DIR/combined_user_simple_random"
cat "$CORPUS_DIR/key_type_logon" "$CORPUS_DIR/desc_numeric" "$CORPUS_DIR/key_data_zeros_16" > "$CORPUS_DIR/combined_logon_numeric_zeros"
cat "$CORPUS_DIR/key_type_keyring" "$CORPUS_DIR/desc_secret" "$CORPUS_DIR/key_data_ones_16" > "$CORPUS_DIR/combined_keyring_secret_ones"

echo "Kernel Keyring Corpus created: $(ls -1 "$CORPUS_DIR" | wc -l) files"
