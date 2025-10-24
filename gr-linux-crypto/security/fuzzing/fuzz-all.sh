#!/bin/bash
# Comprehensive fuzzing script for gr-linux-crypto
# Based on gr-m17 fuzzing infrastructure

set -e

echo "GNU Radio Linux Crypto - Comprehensive Fuzzing Suite"
echo "====================================================="

# Create fuzzing directory
FUZZ_DIR="security/fuzzing/reports/$(date +%Y%m%d_%H%M%S)"
mkdir -p "$FUZZ_DIR"
cd "$FUZZ_DIR"

echo "Working directory: $FUZZ_DIR"

# Check for AFL++ installation
if ! command -v afl-g++ &> /dev/null; then
    echo "ERROR: AFL++ not found. Please install AFL++:"
    echo "  sudo apt-get install afl++"
    exit 1
fi

# Check for required libraries
echo "Checking dependencies..."

# Check for keyutils
if ! ldconfig -p | grep -q libkeyutils; then
    echo "WARNING: libkeyutils not found. Kernel keyring fuzzing may not work."
fi

# Check for OpenSSL
if ! pkg-config --exists openssl; then
    echo "ERROR: OpenSSL not found. Please install OpenSSL development packages."
    exit 1
fi

# Check for libnitrokey (optional)
if ! pkg-config --exists libnitrokey; then
    echo "WARNING: libnitrokey not found. Nitrokey fuzzing will be simulated."
fi

echo "Dependencies check completed."

# Create corpus directories
mkdir -p corpus/{kernel_keyring,kernel_crypto_aes,openssl,nitrokey}
mkdir -p testcases/{kernel_keyring,kernel_crypto_aes,openssl,nitrokey}

# Generate corpus
echo "Generating fuzzing corpus..."
cd ../../corpus_creation

if [ -f "kernel_keyring_corpus.sh" ]; then
    echo "Generating kernel keyring corpus..."
    bash kernel_keyring_corpus.sh
    cp -r kernel_keyring_corpus/* ../reports/$FUZZ_DIR/corpus/kernel_keyring/
fi

if [ -f "kernel_crypto_aes_corpus.sh" ]; then
    echo "Generating kernel crypto AES corpus..."
    bash kernel_crypto_aes_corpus.sh
    cp -r kernel_crypto_aes_corpus/* ../reports/$FUZZ_DIR/corpus/kernel_crypto_aes/
fi

if [ -f "openssl_corpus.sh" ]; then
    echo "Generating OpenSSL corpus..."
    bash openssl_corpus.sh
    cp -r openssl_corpus/* ../reports/$FUZZ_DIR/corpus/openssl/
fi

if [ -f "nitrokey_corpus.sh" ]; then
    echo "Generating Nitrokey corpus..."
    bash nitrokey_corpus.sh
    cp -r nitrokey_corpus/* ../reports/$FUZZ_DIR/corpus/nitrokey/
fi

cd ../reports/$FUZZ_DIR

# Create test cases
echo "Creating test cases..."
printf "test" > testcases/kernel_keyring/simple
printf "user\0test_key\0secret_data" > testcases/kernel_keyring/keyring_test
printf "aes256_key\0iv\0plaintext" > testcases/kernel_crypto_aes/aes_test
printf "openssl_key\0data" > testcases/openssl/openssl_test
printf "\x01\x00test_data" > testcases/nitrokey/nitrokey_test

# Compile fuzzing targets
echo "Compiling fuzzing targets..."

# Kernel keyring fuzzer
if [ -f "../../kernel_keyring_fuzz.cpp" ]; then
    echo "Compiling kernel keyring fuzzer..."
    AFL_FAST_CAL=1 afl-g++ -O2 -funroll-loops -fomit-frame-pointer \
        -o kernel_keyring_fuzz ../../kernel_keyring_fuzz.cpp -lkeyutils
fi

# Kernel crypto AES fuzzer
if [ -f "../../kernel_crypto_aes_fuzz.cpp" ]; then
    echo "Compiling kernel crypto AES fuzzer..."
    AFL_FAST_CAL=1 afl-g++ -O2 -funroll-loops -fomit-frame-pointer \
        -o kernel_crypto_aes_fuzz ../../kernel_crypto_aes_fuzz.cpp
fi

# OpenSSL wrapper fuzzer
if [ -f "../../openssl_wrapper_fuzz.cpp" ]; then
    echo "Compiling OpenSSL wrapper fuzzer..."
    AFL_FAST_CAL=1 afl-g++ -O2 -funroll-loops -fomit-frame-pointer \
        -o openssl_wrapper_fuzz ../../openssl_wrapper_fuzz.cpp -lssl -lcrypto
fi

# Nitrokey interface fuzzer
if [ -f "../../nitrokey_interface_fuzz.cpp" ]; then
    echo "Compiling Nitrokey interface fuzzer..."
    AFL_FAST_CAL=1 afl-g++ -O2 -funroll-loops -fomit-frame-pointer \
        -o nitrokey_interface_fuzz ../../nitrokey_interface_fuzz.cpp
fi

# Create fuzzing execution script
cat > run_fuzzing.sh << 'EOF'
#!/bin/bash
# Run fuzzing for all targets

echo "Starting comprehensive fuzzing..."

# Set AFL++ environment variables
export AFL_SKIP_CPUFREQ=1
export AFL_FAST_CAL=1
export AFL_NO_AFFINITY=1

# Function to run fuzzer
run_fuzzer() {
    local target=$1
    local corpus_dir=$2
    local output_dir=$3
    local timeout=${4:-3600}
    
    if [ -f "$target" ]; then
        echo "Starting fuzzing for $target..."
        timeout $timeout afl-fuzz \
            -i "$corpus_dir" \
            -o "$output_dir" \
            -t 5 \
            -m none \
            -x /dev/null \
            ./"$target" @@ &
        
        local pid=$!
        echo "Fuzzer $target started with PID: $pid"
        echo "$pid" > "${target}.pid"
    else
        echo "WARNING: $target not found, skipping..."
    fi
}

# Run all fuzzers
run_fuzzer "kernel_keyring_fuzz" "corpus/kernel_keyring" "findings_kernel_keyring" 1800
run_fuzzer "kernel_crypto_aes_fuzz" "corpus/kernel_crypto_aes" "findings_kernel_crypto_aes" 1800
run_fuzzer "openssl_wrapper_fuzz" "corpus/openssl" "findings_openssl" 1800
run_fuzzer "nitrokey_interface_fuzz" "corpus/nitrokey" "findings_nitrokey" 1800

echo "All fuzzers started. Monitor with:"
echo "  watch -n 1 \"cat findings_*/default/fuzzer_stats | grep execs_per_sec\""
echo "  tail -f findings_*/default/fuzzer_stats"
EOF

chmod +x run_fuzzing.sh

# Create monitoring script
cat > monitor_fuzzing.sh << 'EOF'
#!/bin/bash
# Monitor fuzzing progress

echo "Fuzzing Progress Monitor"
echo "======================="

while true; do
    clear
    echo "Fuzzing Progress - $(date)"
    echo "=========================="
    
    for dir in findings_*; do
        if [ -d "$dir/default" ]; then
            echo "--- $dir ---"
            if [ -f "$dir/default/fuzzer_stats" ]; then
                grep -E "(execs_per_sec|paths_total|unique_crashes|unique_hangs)" "$dir/default/fuzzer_stats" 2>/dev/null || echo "No stats available"
            else
                echo "No stats file found"
            fi
            echo
        fi
    done
    
    echo "Press Ctrl+C to stop monitoring"
    sleep 5
done
EOF

chmod +x monitor_fuzzing.sh

# Create analysis script
cat > analyze_results.sh << 'EOF'
#!/bin/bash
# Analyze fuzzing results

echo "Fuzzing Results Analysis"
echo "========================"

for dir in findings_*; do
    if [ -d "$dir/default" ]; then
        echo "--- $dir ---"
        
        # Count crashes
        crash_count=$(find "$dir/default/crashes" -name "id:*" 2>/dev/null | wc -l)
        echo "Crashes found: $crash_count"
        
        # Count hangs
        hang_count=$(find "$dir/default/hangs" -name "id:*" 2>/dev/null | wc -l)
        echo "Hangs found: $hang_count"
        
        # Show unique paths
        if [ -f "$dir/default/fuzzer_stats" ]; then
            paths_total=$(grep "paths_total" "$dir/default/fuzzer_stats" | cut -d: -f2 | tr -d ' ')
            echo "Unique paths: $paths_total"
        fi
        
        echo
    fi
done

# Generate summary report
cat > fuzzing_summary.md << 'EOL'
# Fuzzing Results Summary

## Overview
Generated on: $(date)

## Results by Target

EOL

for dir in findings_*; do
    if [ -d "$dir/default" ]; then
        target_name=$(echo "$dir" | sed 's/findings_//')
        crash_count=$(find "$dir/default/crashes" -name "id:*" 2>/dev/null | wc -l)
        hang_count=$(find "$dir/default/hangs" -name "id:*" 2>/dev/null | wc -l)
        
        cat >> fuzzing_summary.md << EOL
### $target_name
- Crashes: $crash_count
- Hangs: $hang_count

EOL
    fi
done

echo "Analysis complete. Summary saved to fuzzing_summary.md"
EOF

chmod +x analyze_results.sh

# Create quick start script
cat > quick_start.sh << 'EOF'
#!/bin/bash
# Quick start fuzzing

echo "Quick Start Fuzzing"
echo "==================="

# Start fuzzing
./run_fuzzing.sh

# Start monitoring in background
./monitor_fuzzing.sh &
MONITOR_PID=$!

# Wait for user to stop
echo "Fuzzing started. Press Ctrl+C to stop."
trap "kill $MONITOR_PID 2>/dev/null; ./analyze_results.sh; exit" INT

# Keep running
while true; do
    sleep 60
done
EOF

chmod +x quick_start.sh

echo ""
echo "Fuzzing setup complete!"
echo "======================"
echo ""
echo "To start fuzzing:"
echo "  ./quick_start.sh"
echo ""
echo "To run individual fuzzers:"
echo "  ./run_fuzzing.sh"
echo ""
echo "To monitor progress:"
echo "  ./monitor_fuzzing.sh"
echo ""
echo "To analyze results:"
echo "  ./analyze_results.sh"
echo ""
echo "Working directory: $FUZZ_DIR"
