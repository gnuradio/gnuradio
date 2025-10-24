# GNU Radio Linux Crypto Fuzzing Infrastructure

This directory contains comprehensive fuzzing harnesses and infrastructure for the GNU Radio Linux Crypto module, adapted from the gr-m17 project's advanced fuzzing framework.

## Overview

The fuzzing infrastructure provides:
- **Fuzzing Harnesses**: Targeted fuzzers for each component
- **Corpus Generation**: Comprehensive test case generation
- **Execution Scripts**: Automated fuzzing execution and monitoring
- **Analysis Tools**: Results analysis and reporting

## Directory Structure

```
security/fuzzing/
├── README.md                           # This file
├── fuzz-all.sh                        # Main fuzzing execution script
├── kernel_keyring_fuzz.cpp            # Kernel keyring fuzzing harness
├── kernel_crypto_aes_fuzz.cpp         # Kernel crypto AES fuzzing harness
├── openssl_wrapper_fuzz.cpp           # OpenSSL wrapper fuzzing harness
├── nitrokey_interface_fuzz.cpp        # Nitrokey interface fuzzing harness
├── corpus_creation/                   # Corpus generation scripts
│   ├── kernel_keyring_corpus.sh       # Kernel keyring corpus
│   ├── kernel_crypto_aes_corpus.sh    # Kernel crypto AES corpus
│   ├── openssl_corpus.sh              # OpenSSL corpus
│   └── nitrokey_corpus.sh             # Nitrokey corpus
└── reports/                           # Fuzzing results (generated)
```

## Fuzzing Harnesses

### 1. Kernel Keyring Fuzzer (`kernel_keyring_fuzz.cpp`)
- **Target**: Linux kernel keyring operations
- **Operations**: Key addition, retrieval, linking, unlinking, revocation
- **Edge Cases**: Invalid keys, corrupted data, boundary conditions
- **Dependencies**: `libkeyutils`

### 2. Kernel Crypto AES Fuzzer (`kernel_crypto_aes_fuzz.cpp`)
- **Target**: Linux kernel crypto API (AF_ALG)
- **Operations**: AES encryption/decryption with various modes
- **Modes**: CBC, ECB, CTR, GCM, XTS
- **Edge Cases**: Weak keys, invalid IVs, malformed data

### 3. OpenSSL Wrapper Fuzzer (`openssl_wrapper_fuzz.cpp`)
- **Target**: OpenSSL cryptographic operations
- **Operations**: AES, hashing, HMAC, RSA
- **Algorithms**: SHA1, SHA256, SHA512, MD5, AES variants
- **Edge Cases**: Invalid keys, corrupted data, algorithm mismatches

### 4. Nitrokey Interface Fuzzer (`nitrokey_interface_fuzz.cpp`)
- **Target**: Hardware security module operations
- **Operations**: Key storage, signing, verification, PIN operations
- **Edge Cases**: Invalid slots, corrupted keys, security violations
- **Note**: Simulated operations (requires actual Nitrokey for real testing)

## Corpus Generation

The corpus generation scripts create comprehensive test cases:

### Kernel Keyring Corpus
- Valid/invalid key types and descriptions
- Various key data sizes and patterns
- Boundary conditions and edge cases
- Corrupted and malformed data

### Kernel Crypto AES Corpus
- Different AES key sizes (128, 192, 256-bit)
- Various IV patterns and plaintext sizes
- Weak keys and boundary conditions
- Combined test cases with realistic scenarios

### OpenSSL Corpus
- Multiple algorithm combinations
- Various data sizes and patterns
- Hash, HMAC, and RSA test cases
- Corrupted and edge case data

### Nitrokey Corpus
- Different operation types and slot numbers
- Various key sizes and PIN patterns
- Signature and verification test cases
- Security-related operations

## Usage

### Prerequisites

```bash
# Install AFL++ (required)
sudo apt-get install afl++

# Install dependencies
sudo apt-get install libkeyutils-dev libssl-dev libnitrokey-dev

# Install GNU Radio development packages
sudo apt-get install gnuradio-dev
```

### Quick Start

```bash
# Run comprehensive fuzzing
cd security/fuzzing
./fuzz-all.sh

# This will:
# 1. Generate corpus for all targets
# 2. Compile fuzzing harnesses
# 3. Create execution scripts
# 4. Start fuzzing with monitoring
```

### Manual Execution

```bash
# Generate corpus
cd corpus_creation
./kernel_keyring_corpus.sh
./kernel_crypto_aes_corpus.sh
./openssl_corpus.sh
./nitrokey_corpus.sh

# Compile fuzzers
afl-g++ -O2 -o kernel_keyring_fuzz kernel_keyring_fuzz.cpp -lkeyutils
afl-g++ -O2 -o kernel_crypto_aes_fuzz kernel_crypto_aes_fuzz.cpp
afl-g++ -O2 -o openssl_wrapper_fuzz openssl_wrapper_fuzz.cpp -lssl -lcrypto
afl-g++ -O2 -o nitrokey_interface_fuzz nitrokey_interface_fuzz.cpp

# Run individual fuzzers
afl-fuzz -i corpus/kernel_keyring -o findings_kernel_keyring ./kernel_keyring_fuzz @@
afl-fuzz -i corpus/kernel_crypto_aes -o findings_kernel_crypto_aes ./kernel_crypto_aes_fuzz @@
afl-fuzz -i corpus/openssl -o findings_openssl ./openssl_wrapper_fuzz @@
afl-fuzz -i corpus/nitrokey -o findings_nitrokey ./nitrokey_interface_fuzz @@
```

### Monitoring and Analysis

```bash
# Monitor fuzzing progress
./monitor_fuzzing.sh

# Analyze results
./analyze_results.sh

# View crash details
ls findings_*/default/crashes/
ls findings_*/default/hangs/
```

## Fuzzing Features

### Advanced Branching
Each fuzzer implements sophisticated branching logic:
- **Size-based branches**: Different code paths for various input sizes
- **Pattern-based branches**: Detection of special data patterns
- **Checksum branches**: Different paths based on data checksums
- **Algorithm-specific branches**: Targeted paths for specific operations

### Edge Case Testing
- **Boundary conditions**: Minimum/maximum input sizes
- **Invalid data**: Corrupted, malformed, or unexpected inputs
- **Security violations**: Weak keys, invalid operations
- **Resource limits**: Large inputs, memory constraints

### Realistic Scenarios
- **Combined operations**: Multiple operations in sequence
- **Real-world data**: Realistic key sizes and patterns
- **Protocol compliance**: Valid protocol structures
- **Error conditions**: Graceful error handling

## Results Analysis

### Crash Analysis
```bash
# View crash details
hexdump -C findings_*/default/crashes/id:*

# Reproduce crashes
./kernel_keyring_fuzz < findings_*/default/crashes/id:*
```

### Performance Metrics
- **Execution rate**: Operations per second
- **Coverage**: Code coverage achieved
- **Unique paths**: Distinct execution paths discovered
- **Crash rate**: Frequency of crashes and hangs

### Security Findings
- **Memory corruption**: Buffer overflows, use-after-free
- **Logic errors**: Incorrect algorithm implementations
- **Resource exhaustion**: Memory leaks, infinite loops
- **Security vulnerabilities**: Weak cryptography, key exposure

## Integration with CI/CD

### Automated Fuzzing
```bash
# Run in CI pipeline
./fuzz-all.sh --timeout 3600 --output results/
```

### Continuous Monitoring
```bash
# Set up continuous fuzzing
crontab -e
# Add: 0 2 * * * cd /path/to/gr-linux-crypto/security/fuzzing && ./fuzz-all.sh
```

## Best Practices

### Fuzzing Strategy
1. **Start with corpus**: Use generated corpus for initial coverage
2. **Monitor progress**: Watch execution rates and coverage
3. **Analyze crashes**: Investigate and fix all crashes
4. **Iterate**: Re-run fuzzing after fixes
5. **Document**: Record findings and fixes

### Performance Optimization
- **Use AFL++**: Latest version with performance improvements
- **Optimize targets**: Compile with optimization flags
- **Tune parameters**: Adjust timeout and memory limits
- **Parallel fuzzing**: Run multiple fuzzer instances

### Security Considerations
- **Sandboxing**: Run fuzzers in isolated environments
- **Resource limits**: Prevent resource exhaustion
- **Access control**: Limit file system and network access
- **Monitoring**: Track fuzzer behavior and resource usage

## Troubleshooting

### Common Issues

1. **AFL++ not found**
   ```bash
   sudo apt-get install afl++
   ```

2. **Missing dependencies**
   ```bash
   sudo apt-get install libkeyutils-dev libssl-dev
   ```

3. **Permission denied**
   ```bash
   sudo sysctl -w kernel.unprivileged_userns_clone=1
   ```

4. **Low execution rate**
   - Check CPU frequency scaling
   - Verify target compilation
   - Review corpus quality

### Debug Mode
```bash
# Enable debug output
export AFL_DEBUG=1
./fuzz-all.sh
```

## Contributing

### Adding New Fuzzers
1. Create new fuzzer in `*_fuzz.cpp`
2. Add corpus generation script
3. Update main execution script
4. Test and validate results

### Improving Corpus
1. Analyze existing corpus effectiveness
2. Add new test cases based on findings
3. Update generation scripts
4. Validate improved coverage

## References

- [AFL++ Documentation](https://github.com/AFLplusplus/AFLplusplus)
- [GNU Radio Security Guidelines](https://wiki.gnuradio.org/index.php/Security)
- [Linux Kernel Crypto API](https://docs.kernel.org/crypto/index.html)
- [OpenSSL Security](https://www.openssl.org/docs/man3.0/man7/security.html)
