#!/usr/bin/env python3
"""
Basic AES encryption example using gr-linux-crypto.
Demonstrates OpenSSL wrapper usage for AES encryption/decryption.
"""

import numpy as np
from gnuradio import gr, blocks
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def main():
    print("GNU Radio Linux Crypto - Basic AES Encryption Example")
    print("=" * 50)
    
    # Initialize crypto helper
    crypto = CryptoHelpers()
    
    # Generate random key and IV
    print("Generating random key and IV...")
    key = crypto.generate_random_key(32)  # 256-bit key
    iv = crypto.generate_random_iv(16)   # 128-bit IV
    
    print(f"Key (hex): {crypto.bytes_to_hex(key)}")
    print(f"IV (hex): {crypto.bytes_to_hex(iv)}")
    
    # Create test data
    test_data = b"Hello, GNU Radio Crypto! This is a test message for encryption."
    print(f"\nOriginal data: {test_data}")
    
    # Encrypt data
    print("\nEncrypting data...")
    encrypted = crypto.aes_encrypt(test_data, key, iv, 'cbc')
    print(f"Encrypted (hex): {crypto.bytes_to_hex(encrypted)}")
    
    # Decrypt data
    print("\nDecrypting data...")
    decrypted = crypto.aes_decrypt(encrypted, key, iv, 'cbc')
    print(f"Decrypted: {decrypted}")
    
    # Verify encryption/decryption worked
    if test_data == decrypted:
        print("\nSUCCESS: Encryption and decryption worked correctly!")
    else:
        print("\nERROR: Encryption/decryption failed!")
    
    # Demonstrate hash operations
    print("\n" + "=" * 50)
    print("Hash Operations")
    print("=" * 50)
    
    hash_sha256 = crypto.hash_data(test_data, 'sha256')
    print(f"SHA256 hash: {crypto.bytes_to_hex(hash_sha256)}")
    
    hash_sha1 = crypto.hash_data(test_data, 'sha1')
    print(f"SHA1 hash: {crypto.bytes_to_hex(hash_sha1)}")
    
    # Demonstrate HMAC
    print("\n" + "=" * 50)
    print("HMAC Operations")
    print("=" * 50)
    
    hmac_sig = crypto.hmac_sign(test_data, key)
    print(f"HMAC signature: {crypto.bytes_to_hex(hmac_sig)}")
    
    # Demonstrate key derivation
    print("\n" + "=" * 50)
    print("Key Derivation")
    print("=" * 50)
    
    password = b"my_secret_password"
    salt = crypto.generate_random_iv(16)
    derived_key = crypto.derive_key_from_password(password, salt)
    print(f"Derived key from password: {crypto.bytes_to_hex(derived_key)}")

if __name__ == "__main__":
    main()

