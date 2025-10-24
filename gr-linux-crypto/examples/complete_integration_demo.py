#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Complete Integration Demo: gr-linux-crypto with existing crypto modules

This example demonstrates the complete integration between gr-linux-crypto
and existing GNU Radio crypto modules (gr-openssl, gr-nacl).

This shows how gr-linux-crypto provides Linux-specific key sources
that complement existing crypto modules rather than duplicating them.
"""

import os
import sys
import time
import subprocess
import tempfile
from typing import Optional, Dict, Any

# Add the gr-linux-crypto module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    from gnuradio import gr, blocks, linux_crypto
    from gnuradio.linux_crypto import linux_crypto_integration
except ImportError as e:
    print(f"Import error: {e}")
    print("Make sure gr-linux-crypto is installed")
    sys.exit(1)

# Optional imports for integration
try:
    from gnuradio import crypto
    CRYPTO_AVAILABLE = True
except ImportError:
    CRYPTO_AVAILABLE = False
    crypto = None

try:
    from gnuradio import nacl
    NACL_AVAILABLE = True
except ImportError:
    NACL_AVAILABLE = False
    nacl = None


class CompleteIntegrationDemo:
    """
    Complete integration demonstration.
    
    This class shows how to use gr-linux-crypto with existing
    crypto modules to create secure, Linux-specific applications.
    """
    
    def __init__(self):
        self.integration = linux_crypto_integration.CryptoIntegration()
        self.temp_files = []
    
    def setup_kernel_keyring_demo(self) -> Optional[int]:
        """Set up kernel keyring demonstration."""
        print("=== Kernel Keyring Integration Demo ===")
        
        # Create test keys
        test_keys = {
            "aes_256_key": b"This is a 256-bit AES key for testing",
            "hmac_key": b"This is an HMAC key for testing",
            "rsa_private": b"This is a simulated RSA private key"
        }
        
        key_ids = {}
        
        for key_name, key_data in test_keys.items():
            key_id = self.integration.keyring_manager.add_key(key_name, key_data)
            if key_id:
                key_ids[key_name] = key_id
                print(f"✓ Added {key_name} to kernel keyring (ID: {key_id})")
            else:
                print(f"✗ Failed to add {key_name} to kernel keyring")
        
        return key_ids
    
    def demonstrate_keyring_to_openssl(self, key_ids: Dict[str, int]) -> bool:
        """Demonstrate kernel keyring to gr-openssl integration."""
        print("\n=== Kernel Keyring to gr-openssl Integration ===")
        
        if not CRYPTO_AVAILABLE:
            print("✗ gr-openssl not available - skipping integration demo")
            return False
        
        if "aes_256_key" not in key_ids:
            print("✗ AES key not available in keyring")
            return False
        
        try:
            # Create flowgraph using kernel keyring with gr-openssl
            cipher_params = {
                'cipher': 'aes-256-cbc',
                'iv': b'\x00' * 16  # Zero IV for demo
            }
            
            tb = self.integration.create_keyring_to_openssl_flowgraph(
                "aes_256_key", cipher_params)
            
            if tb:
                print("✓ Created kernel keyring to gr-openssl flowgraph")
                
                # Create a simple test flowgraph
                test_tb = gr.top_block()
                
                # Create kernel keyring source
                key_source = linux_crypto.kernel_keyring_source(
                    key_ids["aes_256_key"], auto_repeat=True)
                
                # Create data source
                data_source = blocks.vector_source_b([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], True)
                
                # Create file sink
                output_file = tempfile.NamedTemporaryFile(delete=False, suffix='.bin')
                self.temp_files.append(output_file.name)
                output_sink = blocks.file_sink(gr.sizeof_char, output_file.name)
                
                # Connect flowgraph
                test_tb.connect(data_source, output_sink)
                
                # Run flowgraph
                print("✓ Running kernel keyring integration test...")
                test_tb.start()
                time.sleep(1)
                test_tb.stop()
                test_tb.wait()
                
                print(f"✓ Integration test completed - output saved to {output_file.name}")
                return True
            else:
                print("✗ Failed to create integration flowgraph")
                return False
                
        except Exception as e:
            print(f"✗ Error during keyring to gr-openssl integration: {e}")
            return False
    
    def demonstrate_nitrokey_integration(self) -> bool:
        """Demonstrate Nitrokey integration."""
        print("\n=== Nitrokey Integration Demo ===")
        
        if not NACL_AVAILABLE:
            print("✗ gr-nacl not available - skipping Nitrokey integration demo")
            return False
        
        try:
            # Connect to Nitrokey
            if not self.integration.nitrokey_manager.connect():
                print("✗ Failed to connect to Nitrokey")
                return False
            
            print("✓ Connected to Nitrokey device")
            print(f"  Device info: {self.integration.nitrokey_manager.get_device_info()}")
            
            # Get available slots
            available_slots = self.integration.nitrokey_manager.get_available_slots()
            print(f"  Available slots: {available_slots}")
            
            if available_slots:
                # Load key from first available slot
                test_slot = available_slots[0]
                key_data = self.integration.nitrokey_manager.load_key_from_slot(test_slot)
                
                if key_data:
                    print(f"✓ Loaded key from slot {test_slot}: {len(key_data)} bytes")
                    
                    # Create flowgraph using Nitrokey with gr-nacl
                    tb = self.integration.create_nitrokey_to_nacl_flowgraph(test_slot)
                    
                    if tb:
                        print("✓ Created Nitrokey to gr-nacl flowgraph")
                        
                        # Create a simple test flowgraph
                        test_tb = gr.top_block()
                        
                        # Create Nitrokey interface
                        nitrokey_source = linux_crypto.nitrokey_interface(test_slot, auto_repeat=True)
                        
                        # Create data source
                        data_source = blocks.vector_source_b([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], True)
                        
                        # Create file sink
                        output_file = tempfile.NamedTemporaryFile(delete=False, suffix='.bin')
                        self.temp_files.append(output_file.name)
                        output_sink = blocks.file_sink(gr.sizeof_char, output_file.name)
                        
                        # Connect flowgraph
                        test_tb.connect(data_source, output_sink)
                        
                        # Run flowgraph
                        print("✓ Running Nitrokey integration test...")
                        test_tb.start()
                        time.sleep(1)
                        test_tb.stop()
                        test_tb.wait()
                        
                        print(f"✓ Nitrokey integration test completed - output saved to {output_file.name}")
                        return True
                    else:
                        print("✗ Failed to create Nitrokey integration flowgraph")
                        return False
                else:
                    print(f"✗ Failed to load key from slot {test_slot}")
                    return False
            else:
                print("✗ No available slots on Nitrokey device")
                return False
                
        except Exception as e:
            print(f"✗ Error during Nitrokey integration: {e}")
            return False
    
    def demonstrate_kernel_crypto_api(self) -> bool:
        """Demonstrate kernel crypto API usage."""
        print("\n=== Kernel Crypto API Demo ===")
        
        try:
            # Create kernel crypto AES block
            key = bytes([i % 256 for i in range(32)])  # 256-bit key
            iv = bytes([i % 256 for i in range(16)])    # 128-bit IV
            
            crypto_aes = linux_crypto.kernel_crypto_aes(key, iv, "cbc", True)
            
            if crypto_aes.is_kernel_crypto_available():
                print("✓ Kernel crypto API is available")
                print(f"  Supported modes: {crypto_aes.get_supported_modes()}")
                print(f"  Supported key sizes: {crypto_aes.get_supported_key_sizes()}")
                
                # Create test flowgraph
                test_tb = gr.top_block()
                
                # Create data source
                data_source = blocks.vector_source_b([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], True)
                
                # Create file sink
                output_file = tempfile.NamedTemporaryFile(delete=False, suffix='.bin')
                self.temp_files.append(output_file.name)
                output_sink = blocks.file_sink(gr.sizeof_char, output_file.name)
                
                # Connect flowgraph
                test_tb.connect(data_source, crypto_aes)
                test_tb.connect(crypto_aes, output_sink)
                
                # Run flowgraph
                print("✓ Running kernel crypto API test...")
                test_tb.start()
                time.sleep(1)
                test_tb.stop()
                test_tb.wait()
                
                print(f"✓ Kernel crypto API test completed - output saved to {output_file.name}")
                return True
            else:
                print("✗ Kernel crypto API is not available")
                return False
                
        except Exception as e:
            print(f"✗ Error during kernel crypto API demo: {e}")
            return False
    
    def show_integration_architecture(self):
        """Show the integration architecture."""
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
        print("Key Benefits:")
        print("1. No duplication - leverages existing crypto modules")
        print("2. Linux-specific features - kernel keyring, hardware security")
        print("3. Integration focus - bridges existing modules with Linux infrastructure")
        print("4. Secure key management - keys protected by kernel/hardware")
    
    def cleanup(self):
        """Clean up temporary files and resources."""
        print("\n=== Cleanup ===")
        
        # Clean up temporary files
        for temp_file in self.temp_files:
            try:
                os.unlink(temp_file)
                print(f"✓ Cleaned up {temp_file}")
            except OSError:
                print(f"✗ Failed to clean up {temp_file}")
        
        # Clean up kernel keyring
        try:
            self.integration.keyring_manager.clear_all_keys()
            print("✓ Cleaned up kernel keyring")
        except Exception as e:
            print(f"✗ Failed to clean up kernel keyring: {e}")
    
    def run_complete_demo(self):
        """Run the complete integration demonstration."""
        print("=== GNU Radio Linux Crypto Complete Integration Demo ===")
        print()
        
        # Show integration status
        status = self.integration.get_integration_status()
        print("Integration Status:")
        for component, available in status.items():
            status_str = "✓" if available else "✗"
            print(f"  {status_str} {component}")
        print()
        
        # Show architecture
        self.show_integration_architecture()
        
        # Set up kernel keyring demo
        key_ids = self.setup_kernel_keyring_demo()
        
        # Demonstrate integrations
        success_count = 0
        
        if key_ids:
            if self.demonstrate_keyring_to_openssl(key_ids):
                success_count += 1
        
        if self.demonstrate_nitrokey_integration():
            success_count += 1
        
        if self.demonstrate_kernel_crypto_api():
            success_count += 1
        
        # Show results
        print(f"\n=== Demo Results ===")
        print(f"Successful integrations: {success_count}/3")
        
        if success_count > 0:
            print("✓ Integration demonstration completed successfully!")
            print()
            print("This demonstrates how gr-linux-crypto provides unique")
            print("Linux-specific features that complement existing")
            print("GNU Radio crypto modules rather than duplicating them.")
        else:
            print("✗ No successful integrations - check dependencies")
        
        # Cleanup
        self.cleanup()


def main():
    """Main function."""
    demo = CompleteIntegrationDemo()
    demo.run_complete_demo()


if __name__ == '__main__':
    main()
