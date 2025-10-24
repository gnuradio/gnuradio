# GNU Radio Linux Crypto Examples

This document provides comprehensive examples for using the GNU Radio Linux Crypto module.

## Table of Contents

1. [Basic AES Encryption](#basic-aes-encryption)
2. [Kernel Keyring Integration](#kernel-keyring-integration)
3. [Hardware Security Module Usage](#hardware-security-module-usage)
4. [Complete Crypto Flow](#complete-crypto-flow)
5. [Advanced Usage Patterns](#advanced-usage-patterns)

## Basic AES Encryption

### Simple Encryption/Decryption

```python
#!/usr/bin/env python3
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def basic_aes_example():
    """Basic AES encryption example."""
    crypto = CryptoHelpers()
    
    # Generate random key and IV
    key = crypto.generate_random_key(32)  # 256-bit key
    iv = crypto.generate_random_iv(16)    # 128-bit IV
    
    # Encrypt data
    data = b"Hello, GNU Radio Crypto!"
    encrypted = crypto.aes_encrypt(data, key, iv, 'cbc')
    
    # Decrypt data
    decrypted = crypto.aes_decrypt(encrypted, key, iv, 'cbc')
    
    print(f"Original: {data}")
    print(f"Decrypted: {decrypted}")
    print(f"Success: {data == decrypted}")

if __name__ == "__main__":
    basic_aes_example()
```

### GNU Radio Flowgraph Example

```python
#!/usr/bin/env python3
from gnuradio import gr, blocks
from gnuradio import linux_crypto

def gr_aes_flowgraph():
    """GNU Radio AES encryption flowgraph."""
    tb = gr.top_block()
    
    # Create signal source
    src_data = [1, 2, 3, 4, 5] * 1000
    src = blocks.vector_source_b(src_data)
    
    # Create AES encryption block
    key = b'\x01' * 32  # 256-bit key
    iv = b'\x02' * 16   # 128-bit IV
    aes_encrypt = linux_crypto.openssl_wrapper(key, iv, "aes-256-cbc", True)
    
    # Create sink
    sink = blocks.vector_sink_b()
    
    # Connect blocks
    tb.connect(src, aes_encrypt, sink)
    
    # Run flowgraph
    tb.run()
    
    print(f"Processed {len(sink.data())} samples")

if __name__ == "__main__":
    gr_aes_flowgraph()
```

## Kernel Keyring Integration

### Adding and Reading Keys

```python
#!/usr/bin/env python3
from gr_linux_crypto.keyring_helper import KeyringHelper

def keyring_example():
    """Kernel keyring integration example."""
    helper = KeyringHelper()
    
    # Add a key to the keyring
    key_data = b"secret_key_data"
    key_id = helper.add_key('user', 'gr_crypto_key', key_data)
    print(f"Added key with ID: {key_id}")
    
    # Read the key back
    retrieved_data = helper.read_key(key_id)
    print(f"Retrieved data: {retrieved_data}")
    
    # List all keys
    keys = helper.list_keys()
    for key in keys:
        print(f"Key: {key['id']} - {key['description']}")
    
    # Clean up
    helper.revoke_key(key_id)

if __name__ == "__main__":
    keyring_example()
```

### GNU Radio Keyring Source

```python
#!/usr/bin/env python3
from gnuradio import gr, blocks
from gnuradio import linux_crypto

def gr_keyring_flowgraph():
    """GNU Radio keyring source flowgraph."""
    tb = gr.top_block()
    
    # Create keyring source
    keyring_src = linux_crypto.kernel_keyring_source(key_id=12345, auto_repeat=True)
    
    # Create sink
    sink = blocks.vector_sink_b()
    
    # Connect blocks
    tb.connect(keyring_src, sink)
    
    # Run flowgraph
    tb.run()
    
    print(f"Key data: {sink.data()}")

if __name__ == "__main__":
    gr_keyring_flowgraph()
```

## Hardware Security Module Usage

### Nitrokey Integration

```python
#!/usr/bin/env python3
from gnuradio import gr, blocks
from gnuradio import linux_crypto

def nitrokey_example():
    """Nitrokey hardware security module example."""
    # Create Nitrokey interface
    nitrokey = linux_crypto.nitrokey_interface(auto_repeat=False)
    
    # Check if device is connected
    if nitrokey.is_device_connected():
        print("Nitrokey device connected")
        
        # Load key from slot
        if nitrokey.load_key_from_slot(1):
            print("Key loaded from Nitrokey slot 1")
        
        # Create GNU Radio flowgraph
        tb = gr.top_block()
        sink = blocks.vector_sink_b()
        tb.connect(nitrokey, sink)
        tb.run()
        
        print(f"Hardware key data: {sink.data()}")
    else:
        print("No Nitrokey device found")

if __name__ == "__main__":
    nitrokey_example()
```

## Complete Crypto Flow

### End-to-End Encryption Pipeline

```python
#!/usr/bin/env python3
import numpy as np
from gnuradio import gr, blocks
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def complete_crypto_pipeline():
    """Complete cryptographic pipeline example."""
    crypto = CryptoHelpers()
    
    # Generate materials
    key = crypto.generate_random_key(32)
    iv = crypto.generate_random_iv(16)
    
    # Create test message
    message = "GNU Radio Linux Crypto - Secure Communication"
    message_bytes = message.encode('utf-8')
    
    # Pad message
    padded_message = crypto.pad_pkcs7(message_bytes, 16)
    message_array = np.frombuffer(padded_message, dtype=np.uint8)
    
    # Create GNU Radio flowgraph
    tb = gr.top_block()
    
    # Source
    src = blocks.vector_source_b(message_array.tolist())
    
    # Encryption sink
    encrypt_sink = blocks.vector_sink_b()
    
    # Connect and run encryption
    tb.connect(src, encrypt_sink)
    tb.run()
    
    # Get encrypted data
    encrypted_data = np.array(encrypt_sink.data(), dtype=np.uint8)
    
    # Decrypt using crypto helpers
    decrypted_data = crypto.aes_decrypt(encrypted_data.tobytes(), key, iv, 'cbc')
    unpadded_data = crypto.unpad_pkcs7(decrypted_data)
    
    print(f"Original: {message}")
    print(f"Decrypted: {unpadded_data.decode('utf-8')}")
    print(f"Success: {message == unpadded_data.decode('utf-8')}")

if __name__ == "__main__":
    complete_crypto_pipeline()
```

## Advanced Usage Patterns

### Key Derivation and Management

```python
#!/usr/bin/env python3
from gr_linux_crypto.crypto_helpers import CryptoHelpers
from gr_linux_crypto.keyring_helper import KeyringHelper

def advanced_key_management():
    """Advanced key management example."""
    crypto = CryptoHelpers()
    helper = KeyringHelper()
    
    # Derive key from password
    password = b"my_secret_password"
    salt = crypto.generate_random_iv(16)
    derived_key = crypto.derive_key_from_password(password, salt)
    
    # Store derived key in keyring
    key_id = helper.add_key('user', 'derived_key', derived_key)
    
    # Create HMAC
    data = b"Important message"
    hmac_sig = crypto.hmac_sign(data, derived_key)
    
    print(f"Derived key: {crypto.bytes_to_hex(derived_key)}")
    print(f"HMAC: {crypto.bytes_to_hex(hmac_sig)}")
    print(f"Key stored with ID: {key_id}")

if __name__ == "__main__":
    advanced_key_management()
```

### Multi-Algorithm Support

```python
#!/usr/bin/env python3
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def multi_algorithm_example():
    """Multi-algorithm cryptographic operations."""
    crypto = CryptoHelpers()
    
    data = b"Test data for multiple algorithms"
    
    # Hash with different algorithms
    sha1_hash = crypto.hash_data(data, 'sha1')
    sha256_hash = crypto.hash_data(data, 'sha256')
    sha512_hash = crypto.hash_data(data, 'sha512')
    
    print(f"SHA1: {crypto.bytes_to_hex(sha1_hash)}")
    print(f"SHA256: {crypto.bytes_to_hex(sha256_hash)}")
    print(f"SHA512: {crypto.bytes_to_hex(sha512_hash)}")
    
    # AES with different modes
    key = crypto.generate_random_key(32)
    iv = crypto.generate_random_iv(16)
    
    cbc_encrypted = crypto.aes_encrypt(data, key, iv, 'cbc')
    ecb_encrypted = crypto.aes_encrypt(data, key, b'', 'ecb')
    
    print(f"CBC encrypted: {crypto.bytes_to_hex(cbc_encrypted)}")
    print(f"ECB encrypted: {crypto.bytes_to_hex(ecb_encrypted)}")

if __name__ == "__main__":
    multi_algorithm_example()
```

## Performance Considerations

### Benchmarking Crypto Operations

```python
#!/usr/bin/env python3
import time
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def benchmark_crypto():
    """Benchmark cryptographic operations."""
    crypto = CryptoHelpers()
    
    # Test data
    data = b"X" * 1024 * 1024  # 1MB of data
    key = crypto.generate_random_key(32)
    iv = crypto.generate_random_iv(16)
    
    # Benchmark AES encryption
    start_time = time.time()
    encrypted = crypto.aes_encrypt(data, key, iv, 'cbc')
    encryption_time = time.time() - start_time
    
    # Benchmark AES decryption
    start_time = time.time()
    decrypted = crypto.aes_decrypt(encrypted, key, iv, 'cbc')
    decryption_time = time.time() - start_time
    
    print(f"Encryption time: {encryption_time:.3f} seconds")
    print(f"Decryption time: {decryption_time:.3f} seconds")
    print(f"Throughput: {len(data) / encryption_time / 1024 / 1024:.2f} MB/s")

if __name__ == "__main__":
    benchmark_crypto()
```

## Error Handling

### Robust Error Handling

```python
#!/usr/bin/env python3
from gr_linux_crypto.keyring_helper import KeyringHelper
from gr_linux_crypto.crypto_helpers import CryptoHelpers

def error_handling_example():
    """Error handling example."""
    try:
        # Initialize helpers
        helper = KeyringHelper()
        crypto = CryptoHelpers()
        
        # Try to read non-existent key
        try:
            key_data = helper.read_key("999999")
            print(f"Key data: {key_data}")
        except RuntimeError as e:
            print(f"Key not found: {e}")
        
        # Try invalid crypto operation
        try:
            invalid_key = b"short"  # Too short for AES
            crypto.aes_encrypt(b"test", invalid_key, b"iv", 'cbc')
        except Exception as e:
            print(f"Crypto error: {e}")
            
    except Exception as e:
        print(f"Initialization error: {e}")

if __name__ == "__main__":
    error_handling_example()
```

## Best Practices

1. **Key Management**: Always use secure random number generation for keys
2. **Error Handling**: Implement proper exception handling for crypto operations
3. **Memory Management**: Clear sensitive data from memory when done
4. **Performance**: Use appropriate algorithms for your use case
5. **Security**: Follow cryptographic best practices and standards
