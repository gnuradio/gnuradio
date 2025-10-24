#!/bin/bash
# Monitor fuzzing progress for 6-hour session

echo "GNU Radio Linux Crypto - 6-Hour Fuzzing Session Monitor"
echo "======================================================"
echo "Started: $(date)"
echo "Duration: 6 hours (21600 seconds)"
echo ""

# Function to show fuzzer status
show_fuzzer_status() {
    local fuzzer_name=$1
    local findings_dir=$2
    
    echo "--- $fuzzer_name ---"
    if [ -d "$findings_dir/default" ]; then
        if [ -f "$findings_dir/default/fuzzer_stats" ]; then
            echo "Status: Running"
            echo "Executions/sec: $(grep 'execs_per_sec' "$findings_dir/default/fuzzer_stats" | cut -d: -f2 | tr -d ' ' 2>/dev/null || echo 'N/A')"
            echo "Total paths: $(grep 'paths_total' "$findings_dir/default/fuzzer_stats" | cut -d: -f2 | tr -d ' ' 2>/dev/null || echo 'N/A')"
            echo "Unique crashes: $(grep 'unique_crashes' "$findings_dir/default/fuzzer_stats" | cut -d: -f2 | tr -d ' ' 2>/dev/null || echo 'N/A')"
            echo "Unique hangs: $(grep 'unique_hangs' "$findings_dir/default/fuzzer_stats" | cut -d: -f2 | tr -d ' ' 2>/dev/null || echo 'N/A')"
        else
            echo "Status: Starting up..."
        fi
    else
        echo "Status: Not started"
    fi
    echo ""
}

# Monitor loop
while true; do
    clear
    echo "GNU Radio Linux Crypto - 6-Hour Fuzzing Session Monitor"
    echo "======================================================"
    echo "Current time: $(date)"
    echo "Session started: 11:19 (6 hours = until 17:19)"
    echo ""
    
    show_fuzzer_status "Kernel Keyring" "findings_kernel_keyring"
    show_fuzzer_status "Kernel Crypto AES" "findings_kernel_crypto_aes"
    show_fuzzer_status "OpenSSL Wrapper" "findings_openssl"
    show_fuzzer_status "Nitrokey Interface" "findings_nitrokey"
    
    echo "Press Ctrl+C to stop monitoring"
    echo "Fuzzing will continue in background until 6 hours complete"
    sleep 30
done
