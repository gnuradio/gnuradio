#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python helper functions for cryptographic operations.

Provides utilities for GNU Radio crypto operations.
"""

import hashlib
import hmac
import secrets
import base64
from typing import List, Optional, Union, Tuple
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.backends import default_backend
import numpy as np

class CryptoHelpers:
    """Helper class for cryptographic operations."""
    
    @staticmethod
    def generate_random_key(key_size: int = 32) -> bytes:
        """Generate a random key of specified size."""
        return secrets.token_bytes(key_size)
    
    @staticmethod
    def generate_random_iv(iv_size: int = 16) -> bytes:
        """Generate a random IV of specified size."""
        return secrets.token_bytes(iv_size)
    
    @staticmethod
    def hash_data(data: Union[str, bytes], algorithm: str = 'sha256') -> bytes:
        """
        Hash data using specified algorithm.
        
        Args:
            data: Data to hash
            algorithm: Hash algorithm ('sha1', 'sha256', 'sha512')
        
        Returns:
            Hash digest as bytes
        """
        if isinstance(data, str):
            data = data.encode('utf-8')
        
        if algorithm == 'sha1':
            return hashlib.sha1(data).digest()
        elif algorithm == 'sha256':
            return hashlib.sha256(data).digest()
        elif algorithm == 'sha512':
            return hashlib.sha512(data).digest()
        else:
            raise ValueError(f"Unsupported hash algorithm: {algorithm}")
    
    @staticmethod
    def hmac_sign(data: Union[str, bytes], key: bytes, algorithm: str = 'sha256') -> bytes:
        """
        Create HMAC signature.
        
        Args:
            data: Data to sign
            key: HMAC key
            algorithm: Hash algorithm for HMAC
        
        Returns:
            HMAC signature as bytes
        """
        if isinstance(data, str):
            data = data.encode('utf-8')
        
        if algorithm == 'sha1':
            return hmac.new(key, data, hashlib.sha1).digest()
        elif algorithm == 'sha256':
            return hmac.new(key, data, hashlib.sha256).digest()
        elif algorithm == 'sha512':
            return hmac.new(key, data, hashlib.sha512).digest()
        else:
            raise ValueError(f"Unsupported HMAC algorithm: {algorithm}")
    
    @staticmethod
    def aes_encrypt(data: bytes, key: bytes, iv: bytes, mode: str = 'cbc') -> bytes:
        """
        Encrypt data using AES.
        
        Args:
            data: Data to encrypt
            key: AES key (16, 24, or 32 bytes)
            iv: Initialization vector
            mode: AES mode ('cbc', 'ecb', 'cfb', 'ofb')
        
        Returns:
            Encrypted data as bytes
        """
        if mode == 'cbc':
            cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
        elif mode == 'ecb':
            cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
        elif mode == 'cfb':
            cipher = Cipher(algorithms.AES(key), modes.CFB(iv), backend=default_backend())
        elif mode == 'ofb':
            cipher = Cipher(algorithms.AES(key), modes.OFB(iv), backend=default_backend())
        else:
            raise ValueError(f"Unsupported AES mode: {mode}")
        
        encryptor = cipher.encryptor()
        return encryptor.update(data) + encryptor.finalize()
    
    @staticmethod
    def aes_decrypt(data: bytes, key: bytes, iv: bytes, mode: str = 'cbc') -> bytes:
        """
        Decrypt data using AES.
        
        Args:
            data: Data to decrypt
            key: AES key (16, 24, or 32 bytes)
            iv: Initialization vector
            mode: AES mode ('cbc', 'ecb', 'cfb', 'ofb')
        
        Returns:
            Decrypted data as bytes
        """
        if mode == 'cbc':
            cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
        elif mode == 'ecb':
            cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
        elif mode == 'cfb':
            cipher = Cipher(algorithms.AES(key), modes.CFB(iv), backend=default_backend())
        elif mode == 'ofb':
            cipher = Cipher(algorithms.AES(key), modes.OFB(iv), backend=default_backend())
        else:
            raise ValueError(f"Unsupported AES mode: {mode}")
        
        decryptor = cipher.decryptor()
        return decryptor.update(data) + decryptor.finalize()
    
    @staticmethod
    def derive_key_from_password(password: Union[str, bytes], salt: bytes, 
                                length: int = 32, iterations: int = 100000) -> bytes:
        """
        Derive a key from a password using PBKDF2.
        
        Args:
            password: Password to derive key from
            salt: Salt for key derivation
            length: Desired key length in bytes
            iterations: Number of PBKDF2 iterations
        
        Returns:
            Derived key as bytes
        """
        if isinstance(password, str):
            password = password.encode('utf-8')
        
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=length,
            salt=salt,
            iterations=iterations,
            backend=default_backend()
        )
        return kdf.derive(password)
    
    @staticmethod
    def generate_rsa_keypair(key_size: int = 2048) -> Tuple[rsa.RSAPrivateKey, rsa.RSAPublicKey]:
        """
        Generate RSA key pair.
        
        Args:
            key_size: Key size in bits
        
        Returns:
            Tuple of (private_key, public_key)
        """
        private_key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=key_size,
            backend=default_backend()
        )
        public_key = private_key.public_key()
        return private_key, public_key
    
    @staticmethod
    def rsa_encrypt(data: bytes, public_key: rsa.RSAPublicKey) -> bytes:
        """
        Encrypt data using RSA public key.
        
        Args:
            data: Data to encrypt
            public_key: RSA public key
        
        Returns:
            Encrypted data as bytes
        """
        return public_key.encrypt(
            data,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None
            )
        )
    
    @staticmethod
    def rsa_decrypt(data: bytes, private_key: rsa.RSAPrivateKey) -> bytes:
        """
        Decrypt data using RSA private key.
        
        Args:
            data: Data to decrypt
            private_key: RSA private key
        
        Returns:
            Decrypted data as bytes
        """
        return private_key.decrypt(
            data,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None
            )
        )
    
    @staticmethod
    def rsa_sign(data: bytes, private_key: rsa.RSAPrivateKey) -> bytes:
        """
        Sign data using RSA private key.
        
        Args:
            data: Data to sign
            private_key: RSA private key
        
        Returns:
            Signature as bytes
        """
        return private_key.sign(
            data,
            padding.PSS(
                mgf=padding.MGF1(hashes.SHA256()),
                salt_length=padding.PSS.MAX_LENGTH
            ),
            hashes.SHA256()
        )
    
    @staticmethod
    def rsa_verify(data: bytes, signature: bytes, public_key: rsa.RSAPublicKey) -> bool:
        """
        Verify RSA signature.
        
        Args:
            data: Original data
            signature: Signature to verify
            public_key: RSA public key
        
        Returns:
            True if signature is valid, False otherwise
        """
        try:
            public_key.verify(
                signature,
                data,
                padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH
                ),
                hashes.SHA256()
            )
            return True
        except Exception:
            return False
    
    @staticmethod
    def serialize_public_key(public_key: rsa.RSAPublicKey) -> bytes:
        """Serialize RSA public key to PEM format."""
        return public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
    
    @staticmethod
    def serialize_private_key(private_key: rsa.RSAPrivateKey, password: Optional[bytes] = None) -> bytes:
        """Serialize RSA private key to PEM format."""
        if password:
            encryption = serialization.BestAvailableEncryption(password)
        else:
            encryption = serialization.NoEncryption()
        
        return private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=encryption
        )
    
    @staticmethod
    def load_public_key(pem_data: bytes) -> rsa.RSAPublicKey:
        """Load RSA public key from PEM format."""
        return serialization.load_pem_public_key(pem_data, backend=default_backend())
    
    @staticmethod
    def load_private_key(pem_data: bytes, password: Optional[bytes] = None) -> rsa.RSAPrivateKey:
        """Load RSA private key from PEM format."""
        return serialization.load_pem_private_key(
            pem_data, password=password, backend=default_backend()
        )
    
    @staticmethod
    def bytes_to_hex(data: bytes) -> str:
        """Convert bytes to hex string."""
        return data.hex()
    
    @staticmethod
    def hex_to_bytes(hex_str: str) -> bytes:
        """Convert hex string to bytes."""
        return bytes.fromhex(hex_str)
    
    @staticmethod
    def base64_encode(data: bytes) -> str:
        """Encode bytes to base64 string."""
        return base64.b64encode(data).decode('utf-8')
    
    @staticmethod
    def base64_decode(b64_str: str) -> bytes:
        """Decode base64 string to bytes."""
        return base64.b64decode(b64_str)
    
    @staticmethod
    def xor_data(data1: bytes, data2: bytes) -> bytes:
        """XOR two byte sequences."""
        return bytes(a ^ b for a, b in zip(data1, data2))
    
    @staticmethod
    def pad_pkcs7(data: bytes, block_size: int = 16) -> bytes:
        """Pad data using PKCS#7 padding."""
        padding_length = block_size - (len(data) % block_size)
        padding = bytes([padding_length] * padding_length)
        return data + padding
    
    @staticmethod
    def unpad_pkcs7(data: bytes) -> bytes:
        """Remove PKCS#7 padding."""
        padding_length = data[-1]
        return data[:-padding_length]

# GNU Radio specific utilities
class GNURadioCryptoUtils:
    """Utilities specifically for GNU Radio crypto operations."""
    
    @staticmethod
    def numpy_to_bytes(data: np.ndarray) -> bytes:
        """Convert numpy array to bytes."""
        return data.tobytes()
    
    @staticmethod
    def bytes_to_numpy(data: bytes, dtype: np.dtype = np.uint8) -> np.ndarray:
        """Convert bytes to numpy array."""
        return np.frombuffer(data, dtype=dtype)
    
    @staticmethod
    def encrypt_stream_data(data: np.ndarray, key: bytes, iv: bytes, 
                        mode: str = 'cbc') -> np.ndarray:
        """Encrypt numpy array data."""
        data_bytes = data.tobytes()
        encrypted = CryptoHelpers.aes_encrypt(data_bytes, key, iv, mode)
        return np.frombuffer(encrypted, dtype=data.dtype)
    
    @staticmethod
    def decrypt_stream_data(data: np.ndarray, key: bytes, iv: bytes, 
                           mode: str = 'cbc') -> np.ndarray:
        """Decrypt numpy array data."""
        data_bytes = data.tobytes()
        decrypted = CryptoHelpers.aes_decrypt(data_bytes, key, iv, mode)
        return np.frombuffer(decrypted, dtype=data.dtype)

if __name__ == "__main__":
    # Example usage
    crypto = CryptoHelpers()
    
    # Generate random key and IV
    key = crypto.generate_random_key(32)
    iv = crypto.generate_random_iv(16)
    
    # Encrypt some data
    data = b"Hello, GNU Radio Crypto!"
    encrypted = crypto.aes_encrypt(data, key, iv)
    print(f"Encrypted: {crypto.bytes_to_hex(encrypted)}")
    
    # Decrypt the data
    decrypted = crypto.aes_decrypt(encrypted, key, iv)
    print(f"Decrypted: {decrypted}")
    
    # Hash the data
    hash_digest = crypto.hash_data(data)
    print(f"Hash: {crypto.bytes_to_hex(hash_digest)}")
    
    # Create HMAC
    hmac_sig = crypto.hmac_sign(data, key)
    print(f"HMAC: {crypto.bytes_to_hex(hmac_sig)}")
