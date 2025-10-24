#!/bin/bash
# Nitrokey Interface Fuzzing Corpus Generator
CORPUS_DIR="/home/haaken/github-projects/gr-linux-crypto/security/fuzzing/corpus_creation/nitrokey_corpus"
mkdir -p "$CORPUS_DIR"

echo "Creating Nitrokey interface fuzzing corpus..."

# Operation types (first byte)
printf "\x00" > "$CORPUS_DIR/op_connect"
printf "\x01" > "$CORPUS_DIR/op_load_key"
printf "\x02" > "$CORPUS_DIR/op_store_key"
printf "\x03" > "$CORPUS_DIR/op_sign_data"
printf "\x04" > "$CORPUS_DIR/op_verify_signature"
printf "\x05" > "$CORPUS_DIR/op_generate_key"
printf "\x06" > "$CORPUS_DIR/op_get_info"
printf "\x07" > "$CORPUS_DIR/op_list_devices"
printf "\x08" > "$CORPUS_DIR/op_auth_pin"
printf "\x09" > "$CORPUS_DIR/op_change_pin"
printf "\x0A" > "$CORPUS_DIR/op_factory_reset"
printf "\x0B" > "$CORPUS_DIR/op_get_status"
printf "\x0C" > "$CORPUS_DIR/op_backup_keys"
printf "\x0D" > "$CORPUS_DIR/op_restore_keys"
printf "\x0E" > "$CORPUS_DIR/op_test_device"
printf "\x0F" > "$CORPUS_DIR/op_get_firmware"

# Slot numbers (second byte)
printf "\x00" > "$CORPUS_DIR/slot_0"
printf "\x01" > "$CORPUS_DIR/slot_1"
printf "\x02" > "$CORPUS_DIR/slot_2"
printf "\x03" > "$CORPUS_DIR/slot_3"
printf "\x04" > "$CORPUS_DIR/slot_4"
printf "\x05" > "$CORPUS_DIR/slot_5"
printf "\x06" > "$CORPUS_DIR/slot_6"
printf "\x07" > "$CORPUS_DIR/slot_7"
printf "\x08" > "$CORPUS_DIR/slot_8"
printf "\x09" > "$CORPUS_DIR/slot_9"
printf "\x0A" > "$CORPUS_DIR/slot_10"
printf "\x0B" > "$CORPUS_DIR/slot_11"
printf "\x0C" > "$CORPUS_DIR/slot_12"
printf "\x0D" > "$CORPUS_DIR/slot_13"
printf "\x0E" > "$CORPUS_DIR/slot_14"
printf "\x0F" > "$CORPUS_DIR/slot_15"

# Key data variations
dd if=/dev/urandom of="$CORPUS_DIR/key_data_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/key_data_128" bs=128 count=1 2>/dev/null

# Weak keys
printf "\x00%.0s" {1..32} > "$CORPUS_DIR/key_zeros_32"
printf "\xFF%.0s" {1..32} > "$CORPUS_DIR/key_ones_32"
printf "\x55%.0s" {1..32} > "$CORPUS_DIR/key_alternating_32"
printf "\xAA%.0s" {1..32} > "$CORPUS_DIR/key_alternating2_32"

# PIN data
printf "1234" > "$CORPUS_DIR/pin_1234"
printf "0000" > "$CORPUS_DIR/pin_0000"
printf "9999" > "$CORPUS_DIR/pin_9999"
printf "12345678" > "$CORPUS_DIR/pin_long"
printf "" > "$CORPUS_DIR/pin_empty"

# Signature data
dd if=/dev/urandom of="$CORPUS_DIR/signature_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/signature_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/signature_128" bs=128 count=1 2>/dev/null

# Data to sign
dd if=/dev/urandom of="$CORPUS_DIR/data_to_sign_16" bs=16 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/data_to_sign_32" bs=32 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/data_to_sign_64" bs=64 count=1 2>/dev/null
dd if=/dev/urandom of="$CORPUS_DIR/data_to_sign_128" bs=128 count=1 2>/dev/null

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

# Combined test cases for different operations
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/slot_1" "$CORPUS_DIR/key_data_32" > "$CORPUS_DIR/load_key_slot1_32"
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/slot_2" "$CORPUS_DIR/key_data_64" > "$CORPUS_DIR/load_key_slot2_64"
cat "$CORPUS_DIR/op_store_key" "$CORPUS_DIR/slot_3" "$CORPUS_DIR/key_data_128" > "$CORPUS_DIR/store_key_slot3_128"

# Sign operations
cat "$CORPUS_DIR/op_sign_data" "$CORPUS_DIR/slot_1" "$CORPUS_DIR/data_to_sign_32" > "$CORPUS_DIR/sign_data_slot1_32"
cat "$CORPUS_DIR/op_sign_data" "$CORPUS_DIR/slot_2" "$CORPUS_DIR/data_to_sign_64" > "$CORPUS_DIR/sign_data_slot2_64"

# Verify operations
cat "$CORPUS_DIR/op_verify_signature" "$CORPUS_DIR/slot_1" "$CORPUS_DIR/data_to_sign_32" "$CORPUS_DIR/signature_32" > "$CORPUS_DIR/verify_slot1_32"
cat "$CORPUS_DIR/op_verify_signature" "$CORPUS_DIR/slot_2" "$CORPUS_DIR/data_to_sign_64" "$CORPUS_DIR/signature_64" > "$CORPUS_DIR/verify_slot2_64"

# PIN operations
cat "$CORPUS_DIR/op_auth_pin" "$CORPUS_DIR/slot_0" "$CORPUS_DIR/pin_1234" > "$CORPUS_DIR/auth_pin_1234"
cat "$CORPUS_DIR/op_change_pin" "$CORPUS_DIR/slot_0" "$CORPUS_DIR/pin_1234" "$CORPUS_DIR/pin_0000" > "$CORPUS_DIR/change_pin_1234_to_0000"

# Device operations
cp "$CORPUS_DIR/op_connect" "$CORPUS_DIR/connect_device"
cp "$CORPUS_DIR/op_get_info" "$CORPUS_DIR/get_device_info"
cp "$CORPUS_DIR/op_list_devices" "$CORPUS_DIR/list_devices"
cp "$CORPUS_DIR/op_get_status" "$CORPUS_DIR/get_device_status"
cp "$CORPUS_DIR/op_test_device" "$CORPUS_DIR/test_device"
cp "$CORPUS_DIR/op_get_firmware" "$CORPUS_DIR/get_firmware_version"

# Factory reset
cp "$CORPUS_DIR/op_factory_reset" "$CORPUS_DIR/factory_reset"

# Key management
cat "$CORPUS_DIR/op_backup_keys" "$CORPUS_DIR/slot_0" > "$CORPUS_DIR/backup_keys"
cat "$CORPUS_DIR/op_restore_keys" "$CORPUS_DIR/slot_0" "$CORPUS_DIR/key_data_32" > "$CORPUS_DIR/restore_keys"

# Generate key
cat "$CORPUS_DIR/op_generate_key" "$CORPUS_DIR/slot_1" > "$CORPUS_DIR/generate_key_slot1"
cat "$CORPUS_DIR/op_generate_key" "$CORPUS_DIR/slot_2" > "$CORPUS_DIR/generate_key_slot2"

# Edge cases
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/slot_0" "$CORPUS_DIR/single_byte" > "$CORPUS_DIR/load_key_single_byte"
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/slot_15" "$CORPUS_DIR/large_1k" > "$CORPUS_DIR/load_key_large_data"
cat "$CORPUS_DIR/op_sign_data" "$CORPUS_DIR/slot_0" "$CORPUS_DIR/single_byte" > "$CORPUS_DIR/sign_single_byte"

# Corrupted data
dd if=/dev/urandom of="$CORPUS_DIR/corrupted_key" bs=32 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_key" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

dd if=/dev/urandom of="$CORPUS_DIR/corrupted_data" bs=32 count=1 2>/dev/null
printf "\xFF" | dd of="$CORPUS_DIR/corrupted_data" bs=1 count=1 seek=0 conv=notrunc 2>/dev/null

# Combined corrupted test cases
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/slot_1" "$CORPUS_DIR/corrupted_key" > "$CORPUS_DIR/load_corrupted_key"
cat "$CORPUS_DIR/op_sign_data" "$CORPUS_DIR/slot_1" "$CORPUS_DIR/corrupted_data" > "$CORPUS_DIR/sign_corrupted_data"

# Invalid operations
printf "\xFF" > "$CORPUS_DIR/invalid_operation"
printf "\x10" > "$CORPUS_DIR/operation_16"
printf "\x20" > "$CORPUS_DIR/operation_32"

# Invalid slots
cat "$CORPUS_DIR/op_load_key" "$CORPUS_DIR/invalid_operation" "$CORPUS_DIR/key_data_32" > "$CORPUS_DIR/load_invalid_slot"
cat "$CORPUS_DIR/op_sign_data" "$CORPUS_DIR/invalid_operation" "$CORPUS_DIR/data_to_sign_32" > "$CORPUS_DIR/sign_invalid_slot"

echo "Nitrokey Corpus created: $(ls -1 "$CORPUS_DIR" | wc -l) files"
