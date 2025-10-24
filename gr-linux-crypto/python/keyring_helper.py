#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python helper for Linux kernel keyring operations.

Provides a Python interface to keyctl operations for GNU Radio.
"""

import subprocess
import os
import sys
from typing import List, Optional, Union
import tempfile
import base64

class KeyringHelper:
    """Helper class for Linux kernel keyring operations."""

    def __init__(self):
        """Initialize the keyring helper."""
        self.keyctl_path = self._find_keyctl()
        if not self.keyctl_path:
            raise RuntimeError("keyctl command not found. Install keyutils package.")

    def _find_keyctl(self) -> Optional[str]:
        """Find the keyctl binary."""
        for path in ['/usr/bin/keyctl', '/bin/keyctl', '/sbin/keyctl']:
            if os.path.exists(path):
                return path
        return None

    def _run_keyctl(self, args: List[str]) -> subprocess.CompletedProcess:
        """Run keyctl command with given arguments."""
        cmd = [self.keyctl_path] + args
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            return result
        except subprocess.CalledProcessError as e:
            raise RuntimeError(f"keyctl failed: {e.stderr}")
    
    def add_key(self, key_type: str, key_description: str,
                key_data: Union[str, bytes], keyring: str = "@u") -> str:
        """
        Add a key to the keyring.

        Args:
            key_type: Type of key (e.g., 'user', 'logon', 'keyring')
            key_description: Description for the key
            key_data: Key data (string or bytes)
            keyring: Target keyring (default: user keyring)

        Returns:
            Key ID as string
        """
        if isinstance(key_data, str):
            key_data = key_data.encode('utf-8')

        # Create temporary file for key data
        with tempfile.NamedTemporaryFile(mode='wb', delete=False) as f:
            f.write(key_data)
            temp_file = f.name

        try:
            result = self._run_keyctl([
                'add', key_type, key_description, temp_file, keyring
            ])
            return result.stdout.strip()
        finally:
            os.unlink(temp_file)
    
    def request_key(self, key_type: str, key_description: str, 
                   keyring: str = "@u") -> Optional[str]:
        """
        Request a key from the keyring.
        
        Args:
            key_type: Type of key to request
            key_description: Description of the key
            keyring: Source keyring (default: user keyring)
        
        Returns:
            Key ID if found, None otherwise
        """
        try:
            result = self._run_keyctl([
                'request', key_type, key_description, keyring
            ])
            return result.stdout.strip()
        except RuntimeError:
            return None
    
    def read_key(self, key_id: str) -> bytes:
        """
        Read key data from keyring.
        
        Args:
            key_id: Key ID to read
        
        Returns:
            Key data as bytes
        """
        result = self._run_keyctl(['read', key_id])
        return result.stdout.encode('utf-8')
    
    def read_key_hex(self, key_id: str) -> str:
        """
        Read key data as hex string.
        
        Args:
            key_id: Key ID to read
        
        Returns:
            Key data as hex string
        """
        result = self._run_keyctl(['read', key_id])
        return result.stdout.strip()
    
    def list_keys(self, keyring: str = "@u") -> List[dict]:
        """
        List keys in a keyring.
        
        Args:
            keyring: Keyring to list (default: user keyring)
        
        Returns:
            List of key information dictionaries
        """
        result = self._run_keyctl(['list', keyring])
        keys = []
        
        for line in result.stdout.strip().split('\n'):
            if not line:
                continue
            
            parts = line.split()
            if len(parts) >= 3:
                key_id = parts[0]
                key_type = parts[1]
                key_description = ' '.join(parts[2:])
                keys.append({
                    'id': key_id,
                    'type': key_type,
                    'description': key_description
                })
        
        return keys
    
    def search_key(self, key_type: str, key_description: str, 
                   keyring: str = "@u") -> Optional[str]:
        """
        Search for a key in the keyring.
        
        Args:
            key_type: Type of key to search for
            key_description: Description pattern to match
            keyring: Keyring to search (default: user keyring)
        
        Returns:
            Key ID if found, None otherwise
        """
        try:
            result = self._run_keyctl([
                'search', keyring, key_type, key_description
            ])
            return result.stdout.strip()
        except RuntimeError:
            return None
    
    def revoke_key(self, key_id: str) -> bool:
        """
        Revoke a key from the keyring.
        
        Args:
            key_id: Key ID to revoke
        
        Returns:
            True if successful, False otherwise
        """
        try:
            self._run_keyctl(['revoke', key_id])
            return True
        except RuntimeError:
            return False
    
    def unlink_key(self, key_id: str, keyring: str = "@u") -> bool:
        """
        Unlink a key from a keyring.
        
        Args:
            key_id: Key ID to unlink
            keyring: Keyring to unlink from
        
        Returns:
            True if successful, False otherwise
        """
        try:
            self._run_keyctl(['unlink', key_id, keyring])
            return True
        except RuntimeError:
            return False
    
    def create_keyring(self, keyring_name: str, parent_keyring: str = "@u") -> str:
        """
        Create a new keyring.
        
        Args:
            keyring_name: Name for the new keyring
            parent_keyring: Parent keyring (default: user keyring)
        
        Returns:
            New keyring ID
        """
        result = self._run_keyctl([
            'newring', keyring_name, parent_keyring
        ])
        return result.stdout.strip()
    
    def link_key(self, key_id: str, keyring: str) -> bool:
        """
        Link a key to a keyring.
        
        Args:
            key_id: Key ID to link
            keyring: Target keyring
        
        Returns:
            True if successful, False otherwise
        """
        try:
            self._run_keyctl(['link', key_id, keyring])
            return True
        except RuntimeError:
            return False
    
    def get_keyring_id(self, keyring_name: str) -> Optional[str]:
        """
        Get keyring ID by name.
        
        Args:
            keyring_name: Name of the keyring
        
        Returns:
            Keyring ID if found, None otherwise
        """
        try:
            result = self._run_keyctl(['id', keyring_name])
            return result.stdout.strip()
        except RuntimeError:
            return None
    
    def show_key(self, key_id: str) -> dict:
        """
        Show detailed information about a key.
        
        Args:
            key_id: Key ID to show
        
        Returns:
            Dictionary with key information
        """
        result = self._run_keyctl(['show', key_id])
        info = {}
        
        for line in result.stdout.strip().split('\n'):
            if ':' in line:
                key, value = line.split(':', 1)
                info[key.strip()] = value.strip()
        
        return info

# Convenience functions
def add_user_key(description: str, data: Union[str, bytes], keyring: str = "@u") -> str:
    """Add a user key to the keyring."""
    helper = KeyringHelper()
    return helper.add_key('user', description, data, keyring)

def get_user_key(description: str, keyring: str = "@u") -> Optional[bytes]:
    """Get a user key from the keyring."""
    helper = KeyringHelper()
    key_id = helper.search_key('user', description, keyring)
    if key_id:
        return helper.read_key(key_id)
    return None

def list_user_keys(keyring: str = "@u") -> List[dict]:
    """List all user keys in the keyring."""
    helper = KeyringHelper()
    all_keys = helper.list_keys(keyring)
    return [key for key in all_keys if key['type'] == 'user']

if __name__ == "__main__":
    # Example usage
    helper = KeyringHelper()
    
    # Add a test key
    key_id = helper.add_key('user', 'test_key', 'test_data')
    print(f"Added key: {key_id}")
    
    # Read the key back
    key_data = helper.read_key(key_id)
    print(f"Key data: {key_data}")
    
    # List all keys
    keys = helper.list_keys()
    print(f"All keys: {keys}")
