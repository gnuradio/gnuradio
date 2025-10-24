#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Integration example: gr-linux-crypto with gr-openssl

This example demonstrates how to use gr-linux-crypto to provide
kernel keyring keys to gr-openssl for encryption operations.

This shows the integration approach - we don't duplicate gr-openssl
functionality, we provide Linux-specific key sources for it.
"""

import os
import sys
import time
import subprocess

# Add the gr-linux-crypto module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    from gnuradio import gr, blocks, crypto, linux_crypto
    from gnuradio.crypto import sym_ciph_desc
except ImportError as e:
    print(f"Import error: {e}")
    print("Make sure gr-openssl and gr-linux-crypto are installed")
    sys.exit(1)


def setup_kernel_keyring():
    """Set up a test key in the kernel keyring"""
    print("Setting up kernel keyring with test key...")
    
    # Create a test key in the kernel keyring
    key_data = b"This is a test key for gr-linux-crypto integration"
    
    # Use keyctl to add the key
    try:
        result = subprocess.run([
            'keyctl', 'add', 'user', 'gr_crypto_test_key', 
            key_data.decode(), '@u'
        ], capture_output=True, text=True, check=True)
        
        key_id = int(result.stdout.strip())
        print(f"Added key to kernel keyring with ID: {key_id}")
        return key_id
        
    except subprocess.CalledProcessError as e:
        print(f"Failed to add key to kernel keyring: {e}")
        print("Make sure you have keyctl installed and proper permissions")
        return None


def create_flowgraph(key_id):
    """Create a GNU Radio flowgraph using kernel keyring with gr-openssl"""
    
    print(f"Creating flowgraph with key ID: {key_id}")
    
    # Create top block
    tb = gr.top_block()
    
    # Create kernel keyring source (gr-linux-crypto)
    key_source = linux_crypto.kernel_keyring_source(key_id, auto_repeat=True)
    
    # Create a simple data source for testing
    data_source = blocks.vector_source_b([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], True)
    
    # Create OpenSSL cipher descriptor (gr-openssl)
    # Note: This is a simplified example - real usage would need proper IV handling
    cipher_desc = sym_ciph_desc("aes-256-cbc", b"", b"")  # Empty key/IV for now
    
    # Create OpenSSL encryptor (gr-openssl)
    encryptor = crypto.sym_enc(cipher_desc)
    
    # Create file sink for output
    output_sink = blocks.file_sink(gr.sizeof_char, "encrypted_output.bin")
    
    # Connect the flowgraph
    # Note: This is a conceptual example - real implementation would need
    # proper message passing between kernel keyring source and OpenSSL
    tb.connect(data_source, encryptor)
    tb.connect(encryptor, output_sink)
    
    return tb


def demonstrate_integration():
    """Demonstrate the integration between gr-linux-crypto and gr-openssl"""
    
    print("=== GNU Radio Linux Crypto Integration Demo ===")
    print("This demonstrates how gr-linux-crypto provides Linux-specific")
    print("key sources for existing crypto modules like gr-openssl.")
    print()
    
    # Set up kernel keyring
    key_id = setup_kernel_keyring()
    if key_id is None:
        print("Failed to set up kernel keyring. Exiting.")
        return
    
    try:
        # Create and run flowgraph
        print("Creating integration flowgraph...")
        tb = create_flowgraph(key_id)
        
        print("Starting flowgraph...")
        tb.start()
        
        # Let it run for a short time
        time.sleep(2)
        
        print("Stopping flowgraph...")
        tb.stop()
        tb.wait()
        
        print("Integration demo completed successfully!")
        print()
        print("Key benefits of this integration:")
        print("1. Keys stored securely in kernel keyring")
        print("2. No duplication of gr-openssl functionality")
        print("3. Linux-specific features complement existing modules")
        print("4. Secure key management for GNU Radio applications")
        
    except Exception as e:
        print(f"Error during integration demo: {e}")
        
    finally:
        # Clean up kernel keyring
        try:
            subprocess.run(['keyctl', 'unlink', str(key_id), '@u'], 
                          capture_output=True, check=True)
            print(f"Cleaned up key {key_id} from kernel keyring")
        except subprocess.CalledProcessError:
            print("Failed to clean up kernel keyring key")


def show_architecture():
    """Show the integration architecture"""
    print("\n=== Integration Architecture ===")
    print()
    print("┌─────────────────────────────────────────────────────────────┐")
    print("│                    GNU Radio Application                    │")
    print("└─────────────────────┬───────────────────────────────────────┘")
    print("                      │")
    print("┌─────────────────────▼───────────────────────────────────────┐")
    print("│                Integration Layer                           │")
    print("│  ┌─────────────────┐ ┌─────────────────┐ ┌──────────────┐ │")
    print("│  │ gr-openssl      │ │ gr-nacl         │ │ gr-linux-    │ │")
    print("│  │ (OpenSSL ops)   │ │ (Modern crypto) │ │ crypto       │ │")
    print("│  └─────────────────┘ └─────────────────┘ └──────────────┘ │")
    print("└─────────────────────┬───────────────────────────────────────┘")
    print("                      │")
    print("┌─────────────────────▼───────────────────────────────────────┐")
    print("│                Linux-Specific Layer                        │")
    print("│  ┌─────────────────┐ ┌─────────────────┐ ┌──────────────┐ │")
    print("│  │ Kernel Keyring │ │ Hardware        │ │ Kernel       │ │")
    print("│  │ (Secure keys)  │ │ Security        │ │ Crypto API   │ │")
    print("│  └─────────────────┘ └─────────────────┘ └──────────────┘ │")
    print("└─────────────────────────────────────────────────────────────┘")
    print()
    print("This module provides Linux-specific features that complement")
    print("existing crypto modules rather than duplicating them.")


if __name__ == '__main__':
    show_architecture()
    demonstrate_integration()
