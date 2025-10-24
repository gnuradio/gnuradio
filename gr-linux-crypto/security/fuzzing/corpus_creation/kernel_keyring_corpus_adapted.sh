#!/bin/bash
# Kernel Keyring Fuzzing Corpus Generator - Adapted from gr-m17 patterns
CORPUS_DIR="/home/haaken/github-projects/gr-linux-crypto/security/fuzzing/corpus_creation/kernel_keyring_corpus"
mkdir -p "$CORPUS_DIR"

echo "Creating Kernel Keyring fuzzing corpus (adapted from gr-m17 patterns)..."

# Operation types (first byte) - adapted from gr-m17 operation patterns
printf "\x00" > "$CORPUS_DIR/op_add_key"
printf "\x01" > "$CORPUS_DIR/op_search_key"
printf "\x02" > "$CORPUS_DIR/op_read_key"
printf "\x03" > "$CORPUS_DIR/op_link_key"
printf "\x04" > "$CORPUS_DIR/op_unlink_key"
printf "\x05" > "$CORPUS_DIR/op_revoke_key"
printf "\x06" > "$CORPUS_DIR/op_set_permissions"
printf "\x07" > "$CORPUS_DIR/op_get_permissions"
printf "\x08" > "$CORPUS_DIR/op_describe_key"
printf "\x09" > "$CORPUS_DIR/op_list_keys"
printf "\x0A" > "$CORPUS_DIR/op_create_keyring"
printf "\x0B" > "$CORPUS_DIR/op_update_key"
printf "\x0C" > "$CORPUS_DIR/op_clear_keyring"
printf "\x0D" > "$CORPUS_DIR/op_invalidate_key"
printf "\x0E" > "$CORPUS_DIR/op_get_keyring_id"
printf "\x0F" > "$CORPUS_DIR/op_join_session"

# Key types (second byte) - following gr-m17 bit pattern approach
printf "\x00" > "$CORPUS_DIR/key_type_user"
printf "\x01" > "$CORPUS_DIR/key_type_logon"
printf "\x02" > "$CORPUS_DIR/key_type_keyring"
printf "\x04" > "$CORPUS_DIR/key_type_big_key"

# Valid key descriptions (bytes 2-32) - following gr-m17 string patterns
printf "gr_crypto_test_key\0" > "$CORPUS_DIR/desc_gr_crypto"
printf "test_key_123\0" > "$CORPUS_DIR/desc_numeric"
printf "my_secret_key\0" > "$CORPUS_DIR/desc_secret"
printf "key_with_underscores\0" > "$CORPUS_DIR/desc_underscores"
printf "key-with-dashes\0" > "$CORPUS_DIR/desc_dashes"
printf "key.with.dots\0" > "$CORPUS_DIR/desc_dots"

# Key data variations (remaining bytes) - following gr-m17 data patterns
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_random_128" bs=128 count=1 2>/dev/null

# Weak keys - following gr-m17 weak key patterns
printf "\x00%.0s" {1..16} > "$CORPUS_DIR/key_data_zeros_16"
printf "\xFF%.0s" {1..16} > "$CORPUS_DIR/key_data_ones_16"
printf "\x55%.0s" {1..16} > "$CORPUS_DIR/key_data_alternating_16"
printf "\xAA%.0s" {1..16} > "$CORPUS_DIR/key_data_alternating2_16"

# Sequential keys - following gr-m17 sequential patterns
printf "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > "$CORPUS_DIR/key_data_sequential_16"
printf "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" > "$CORPUS_DIR/key_data_sequential2_16"

# Boundary sizes - following gr-m17 boundary patterns
printf "\x00" > "$CORPUS_DIR/key_data_single_byte"
printf "\x00\x00" > "$CORPUS_DIR/key_data_two_bytes"
dd if=/dev/urandom of="$CORPUS_DIR/key_data_max" bs=1024 count=1 2>/dev/null

# Special characters in descriptions - following gr-m17 special char patterns
printf "key_with_special_chars_!@#$%%^&*()\0" > "$CORPUS_DIR/desc_special_chars"
printf "key_with_unicode_测试\0" > "$CORPUS_DIR/desc_unicode"
printf "key_with_spaces and more\0" > "$CORPUS_DIR/desc_spaces"
printf "key_with_tabs\tand\tmore\0" > "$CORPUS_DIR/desc_tabs"

# Empty and null descriptions - following gr-m17 null patterns
printf "\0" > "$CORPUS_DIR/desc_empty"
printf "\x00\x00\x00\x00" > "$CORPUS_DIR/desc_multiple_nulls"

# Long descriptions - following gr-m17 long string patterns
printf "a%.0s" {1..255} > "$CORPUS_DIR/desc_long_255"
printf "a%.0s" {1..1000} > "$CORPUS_DIR/desc_very_long"

# Key data with special patterns - following gr-m17 hex patterns
printf "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10" > "$CORPUS_DIR/key_data_hex_pattern"
printf "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE" > "$CORPUS_DIR/key_data_deadbeef"

# Corrupted data - following gr-m17 corruption patterns
dd if=/dev/urandom of="$CORPUS_DIR/key_data_corrupted" bs=32 count=1 2>/dev/null
# Flip some bits
dd if="$CORPUS_DIR/key_data_corrupted" of="$CORPUS_DIR/key_data_corrupted_flipped" bs=1 count=32 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/key_data_corrupted_flipped" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

# Combined test cases (operation + key_type + description + data) - following gr-m17 combined patterns
cat "$CORPUS_DIR/op_add_key" "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_gr_crypto" "$CORPUS_DIR/key_data_random_32" > "$CORPUS_DIR/combined_add_user_gr_crypto_random"
cat "$CORPUS_DIR/op_search_key" "$CORPUS_DIR/key_type_logon" "$CORPUS_DIR/desc_numeric" "$CORPUS_DIR/key_data_zeros_16" > "$CORPUS_DIR/combined_search_logon_numeric_zeros"
cat "$CORPUS_DIR/op_link_key" "$CORPUS_DIR/key_type_keyring" "$CORPUS_DIR/desc_secret" "$CORPUS_DIR/key_data_ones_16" > "$CORPUS_DIR/combined_link_keyring_secret_ones"

# Edge case combinations - following gr-m17 edge case patterns
cat "$CORPUS_DIR/op_add_key" "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_empty" "$CORPUS_DIR/key_data_single_byte" > "$CORPUS_DIR/edge_case_add_empty_single"
cat "$CORPUS_DIR/op_read_key" "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_gr_crypto" "$CORPUS_DIR/key_data_max" > "$CORPUS_DIR/edge_case_read_large_data"
cat "$CORPUS_DIR/op_revoke_key" "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_special_chars" "$CORPUS_DIR/key_data_corrupted" > "$CORPUS_DIR/edge_case_revoke_special_corrupted"

# Invalid operation combinations - following gr-m17 invalid patterns
printf "\xFF" > "$CORPUS_DIR/invalid_operation"
printf "\x10" > "$CORPUS_DIR/operation_16"
printf "\x20" > "$CORPUS_DIR/operation_32"

# Combined invalid test cases
cat "$CORPUS_DIR/invalid_operation" "$CORPUS_DIR/key_type_user" "$CORPUS_DIR/desc_gr_crypto" "$CORPUS_DIR/key_data_random_32" > "$CORPUS_DIR/invalid_operation_test"
cat "$CORPUS_DIR/op_add_key" "$CORPUS_DIR/invalid_operation" "$CORPUS_DIR/desc_gr_crypto" "$CORPUS_DIR/key_data_random_32" > "$CORPUS_DIR/invalid_key_type_test"

echo "Kernel Keyring Corpus created: $(ls -1 "$CORPUS_DIR" | wc -l) files"
