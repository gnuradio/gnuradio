#!/usr/bin/env python3
"""
Kernel keyring integration example.
Demonstrates reading keys from Linux kernel keyring.
"""

import sys
import time
from gnuradio import gr, blocks
from gr_linux_crypto.keyring_helper import KeyringHelper

def main():
    print("GNU Radio Linux Crypto - Kernel Keyring Example")
    print("=" * 50)
    
    try:
        # Initialize keyring helper
        helper = KeyringHelper()
        print("Keyring helper initialized successfully")
        
        # Add a test key to the keyring
        print("\nAdding test key to kernel keyring...")
        test_key_data = b"This is a secret key for GNU Radio crypto operations"
        key_id = helper.add_key('user', 'gr_crypto_test_key', test_key_data)
        print(f"Added key with ID: {key_id}")
        
        # List all keys in the keyring
        print("\nListing keys in keyring...")
        keys = helper.list_keys()
        for key in keys:
            print(f"  ID: {key['id']}, Type: {key['type']}, Description: {key['description']}")
        
        # Read the key back
        print(f"\nReading key {key_id} from keyring...")
        key_data = helper.read_key(key_id)
        print(f"Key data: {key_data}")
        
        # Verify the key data
        if key_data == test_key_data:
            print("SUCCESS: Key data matches original!")
        else:
            print("ERROR: Key data does not match!")
        
        # Demonstrate GNU Radio integration
        print("\n" + "=" * 50)
        print("GNU Radio Integration")
        print("=" * 50)
        
        # Create a simple GNU Radio flowgraph
        tb = gr.top_block()
        
        # Create a signal source (simulate data)
        src_data = [1, 2, 3, 4, 5] * 100
        src = blocks.vector_source_b(src_data)
        
        # Create a sink to collect data
        sink = blocks.vector_sink_b()
        
        # Connect the blocks
        tb.connect(src, sink)
        
        print("Running GNU Radio flowgraph...")
        tb.run()
        
        print(f"Processed {len(sink.data())} samples")
        print(f"First 10 samples: {sink.data()[:10]}")
        
        # Clean up - remove the test key
        print(f"\nCleaning up - removing key {key_id}...")
        if helper.revoke_key(key_id):
            print("Key removed successfully")
        else:
            print("Failed to remove key")
            
    except Exception as e:
        print(f"Error: {e}")
        print("\nTroubleshooting tips:")
        print("1. Ensure keyutils package is installed: sudo apt-get install keyutils")
        print("2. Check if you have permission to access the keyring")
        print("3. Try running with sudo if permission issues persist")
        sys.exit(1)

if __name__ == "__main__":
    main()

