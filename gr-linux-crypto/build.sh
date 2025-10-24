#!/bin/bash
# Build script for gr-linux-crypto

set -e

echo "Building gr-linux-crypto module..."

# Check dependencies
echo "Checking dependencies..."

# Check for GNU Radio
if ! pkg-config --exists gnuradio-runtime; then
    echo "ERROR: GNU Radio not found. Please install GNU Radio development packages."
    exit 1
fi

# Check for OpenSSL
if ! pkg-config --exists openssl; then
    echo "ERROR: OpenSSL not found. Please install OpenSSL development packages."
    exit 1
fi

# Check for keyutils
if ! ldconfig -p | grep -q libkeyutils; then
    echo "WARNING: libkeyutils not found. Kernel keyring features may not work."
fi

# Check for libnitrokey
if ! pkg-config --exists libnitrokey; then
    echo "WARNING: libnitrokey not found. Nitrokey features may not work."
fi

echo "Dependencies check completed."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_CXX_STANDARD=17

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "To install:"
echo "  sudo make install"
echo ""
echo "To run examples:"
echo "  cd ../examples"
echo "  python3 basic_aes_encryption.py"
echo "  python3 kernel_keyring_example.py"

