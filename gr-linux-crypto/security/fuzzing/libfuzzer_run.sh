#!/bin/bash

# LibFuzzer run script for gr-linux-crypto
# This script runs LibFuzzer tests for 6 hours

set -e

echo "=== LibFuzzer Run Script ==="
echo "Starting LibFuzzer tests for 6 hours..."

# Create reports directory
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
REPORTS_DIR="reports/libfuzzer_$TIMESTAMP"
mkdir -p "$REPORTS_DIR"
cd "$REPORTS_DIR"

echo "Reports directory: $REPORTS_DIR"

# Create corpus directories
mkdir -p corpus_kernel_keyring
mkdir -p corpus_kernel_crypto_aes
mkdir -p corpus_nitrokey
mkdir -p corpus_openssl

# Generate seed corpus
echo "Generating seed corpus..."
for corpus_dir in corpus_*; do
    echo "Creating corpus for $corpus_dir..."
    
    # Create diverse test cases
    for i in {1..50}; do
        # Small inputs
        dd if=/dev/urandom bs=1 count=$((RANDOM % 16 + 1)) of="$corpus_dir/input_${i}_small" 2>/dev/null
        
        # Medium inputs
        dd if=/dev/urandom bs=1 count=$((RANDOM % 64 + 16)) of="$corpus_dir/input_${i}_medium" 2>/dev/null
        
        # Large inputs
        dd if=/dev/urandom bs=1 count=$((RANDOM % 256 + 64)) of="$corpus_dir/input_${i}_large" 2>/dev/null
        
        # Pattern-based inputs
        printf "%0${i}d" 0 > "$corpus_dir/pattern_${i}_zeros"
        printf "%0${i}x" 0 > "$corpus_dir/pattern_${i}_hex"
    done
    
    echo "Created $(ls -1 "$corpus_dir" | wc -l) test cases in $corpus_dir"
done

echo ""
echo "=== Starting LibFuzzer Tests ==="

# Run kernel keyring fuzzer
echo "Starting kernel_keyring_libfuzzer..."
timeout 6h ../../libfuzzer_build/kernel_keyring_libfuzzer corpus_kernel_keyring > kernel_keyring.log 2>&1 &
KERNEL_KEYRING_PID=$!

# Run kernel crypto AES fuzzer
echo "Starting kernel_crypto_aes_libfuzzer..."
timeout 6h ../../libfuzzer_build/kernel_crypto_aes_libfuzzer corpus_kernel_crypto_aes > kernel_crypto_aes.log 2>&1 &
KERNEL_CRYPTO_AES_PID=$!

# Run nitrokey fuzzer
echo "Starting nitrokey_libfuzzer..."
timeout 6h ../../libfuzzer_build/nitrokey_libfuzzer corpus_nitrokey > nitrokey.log 2>&1 &
NITROKEY_PID=$!

# Run OpenSSL fuzzer
echo "Starting openssl_libfuzzer..."
timeout 6h ../../libfuzzer_build/openssl_libfuzzer corpus_openssl > openssl.log 2>&1 &
OPENSSL_PID=$!

echo ""
echo "=== LibFuzzer Tests Started ==="
echo "Process IDs:"
echo "  kernel_keyring: $KERNEL_KEYRING_PID"
echo "  kernel_crypto_aes: $KERNEL_CRYPTO_AES_PID"
echo "  nitrokey: $NITROKEY_PID"
echo "  openssl: $OPENSSL_PID"

echo ""
echo "=== Monitoring Progress ==="
echo "Checking progress every 30 seconds..."

# Monitor progress
for i in {1..720}; do  # 6 hours = 720 * 30 seconds
    echo "--- Progress Check $i ---"
    echo "Time: $(date)"
    
    # Check if processes are still running
    if ps -p $KERNEL_KEYRING_PID > /dev/null 2>&1; then
        echo "kernel_keyring: Running"
    else
        echo "kernel_keyring: Stopped"
    fi
    
    if ps -p $KERNEL_CRYPTO_AES_PID > /dev/null 2>&1; then
        echo "kernel_crypto_aes: Running"
    else
        echo "kernel_crypto_aes: Stopped"
    fi
    
    if ps -p $NITROKEY_PID > /dev/null 2>&1; then
        echo "nitrokey: Running"
    else
        echo "nitrokey: Stopped"
    fi
    
    if ps -p $OPENSSL_PID > /dev/null 2>&1; then
        echo "openssl: Running"
    else
        echo "openssl: Stopped"
    fi
    
    # Check log sizes
    echo "Log sizes:"
    for log in *.log; do
        if [ -f "$log" ]; then
            echo "  $log: $(stat -c%s "$log") bytes"
        fi
    done
    
    echo ""
    sleep 30
done

echo ""
echo "=== LibFuzzer Tests Complete ==="
echo "Final status:"
echo "Time: $(date)"

# Final process check
echo "Final process status:"
if ps -p $KERNEL_KEYRING_PID > /dev/null 2>&1; then
    echo "kernel_keyring: Still running"
else
    echo "kernel_keyring: Completed"
fi

if ps -p $KERNEL_CRYPTO_AES_PID > /dev/null 2>&1; then
    echo "kernel_crypto_aes: Still running"
else
    echo "kernel_crypto_aes: Completed"
fi

if ps -p $NITROKEY_PID > /dev/null 2>&1; then
    echo "nitrokey: Still running"
else
    echo "nitrokey: Completed"
fi

if ps -p $OPENSSL_PID > /dev/null 2>&1; then
    echo "openssl: Still running"
else
    echo "openssl: Completed"
fi

echo ""
echo "=== LibFuzzer Run Complete ==="
echo "Reports saved in: $REPORTS_DIR"
