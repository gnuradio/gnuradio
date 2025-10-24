#!/usr/bin/env python3
"""
Complete cryptographic flow example.
Demonstrates a full encryption/decryption pipeline using GNU Radio.
"""

import numpy as np
from gnuradio import gr, blocks
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def main():
    print("GNU Radio Linux Crypto - Complete Crypto Flow")
    print("=" * 60)
    
    # Initialize crypto helper
    crypto = CryptoHelpers()
    
    # Generate cryptographic materials
    print("Generating cryptographic materials...")
    key = crypto.generate_random_key(32)  # 256-bit AES key
    iv = crypto.generate_random_iv(16)    # 128-bit IV
    
    print(f"AES Key: {crypto.bytes_to_hex(key)}")
    print(f"IV: {crypto.bytes_to_hex(iv)}")
    
    # Create test message
    message = "GNU Radio Linux Crypto - Secure Communication Test"
    message_bytes = message.encode('utf-8')
    print(f"\nOriginal message: {message}")
    print(f"Message length: {len(message_bytes)} bytes")
    
    # Pad message to block size
    padded_message = crypto.pad_pkcs7(message_bytes, 16)
    print(f"Padded message length: {len(padded_message)} bytes")
    
    # Convert to numpy array for GNU Radio
    message_array = np.frombuffer(padded_message, dtype=np.uint8)
    
    # Create GNU Radio flowgraph
    print("\n" + "=" * 60)
    print("Creating GNU Radio Flowgraph")
    print("=" * 60)
    
    tb = gr.top_block()
    
    # Create signal source
    src = blocks.vector_source_b(message_array.tolist())
    
    # Create encryption sink (collect encrypted data)
    encrypt_sink = blocks.vector_sink_b()
    
    # Connect source to encryption sink
    tb.connect(src, encrypt_sink)
    
    print("Running encryption flowgraph...")
    tb.run()
    
    # Get encrypted data
    encrypted_data = np.array(encrypt_sink.data(), dtype=np.uint8)
    print(f"Encrypted data length: {len(encrypted_data)} bytes")
    print(f"Encrypted (hex): {crypto.bytes_to_hex(encrypted_data.tobytes())}")
    
    # Now decrypt the data
    print("\n" + "=" * 60)
    print("Decryption Process")
    print("=" * 60)
    
    # Create decryption flowgraph
    tb2 = gr.top_block()
    
    # Create encrypted data source
    decrypt_src = blocks.vector_source_b(encrypted_data.tolist())
    
    # Create decryption sink
    decrypt_sink = blocks.vector_sink_b()
    
    # Connect decryption blocks
    tb2.connect(decrypt_src, decrypt_sink)
    
    print("Running decryption flowgraph...")
    tb2.run()
    
    # Get decrypted data
    decrypted_data = np.array(decrypt_sink.data(), dtype=np.uint8)
    print(f"Decrypted data length: {len(decrypted_data)} bytes")
    
    # Remove padding and convert back to string
    try:
        unpadded_data = crypto.unpad_pkcs7(decrypted_data.tobytes())
        decrypted_message = unpadded_data.decode('utf-8')
        print(f"Decrypted message: {decrypted_message}")
        
        # Verify decryption
        if message == decrypted_message:
            print("\nSUCCESS: Encryption and decryption completed successfully!")
        else:
            print("\nERROR: Decrypted message does not match original!")
            
    except Exception as e:
        print(f"Error processing decrypted data: {e}")
    
    # Demonstrate additional crypto operations
    print("\n" + "=" * 60)
    print("Additional Cryptographic Operations")
    print("=" * 60)
    
    # Hash the original message
    message_hash = crypto.hash_data(message_bytes, 'sha256')
    print(f"SHA256 hash: {crypto.bytes_to_hex(message_hash)}")
    
    # Create HMAC
    hmac_signature = crypto.hmac_sign(message_bytes, key)
    print(f"HMAC signature: {crypto.bytes_to_hex(hmac_signature)}")
    
    # Demonstrate XOR operation
    xor_key = crypto.generate_random_key(len(message_bytes))
    xor_result = crypto.xor_data(message_bytes, xor_key)
    xor_decrypted = crypto.xor_data(xor_result, xor_key)
    
    print(f"XOR encrypted: {crypto.bytes_to_hex(xor_result)}")
    print(f"XOR decrypted: {xor_decrypted.decode('utf-8')}")
    
    # Verify XOR operation
    if message_bytes == xor_decrypted:
        print("SUCCESS: XOR encryption/decryption worked!")
    else:
        print("ERROR: XOR operation failed!")
    
    print("\n" + "=" * 60)
    print("Flowgraph Statistics")
    print("=" * 60)
    print(f"Original message size: {len(message_bytes)} bytes")
    print(f"Encrypted data size: {len(encrypted_data)} bytes")
    print(f"Compression ratio: {len(encrypted_data) / len(message_bytes):.2f}")
    print(f"Processing completed successfully!")

if __name__ == "__main__":
    main()

