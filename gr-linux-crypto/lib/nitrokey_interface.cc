#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <libnitrokey/NitrokeyManager.h>
#include <libnitrokey/CommandFailedException.h>
#include <libnitrokey/DeviceNotConnectedException.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>

namespace gr {
namespace linux_crypto {

class nitrokey_interface : public gr::block
{
private:
    std::unique_ptr<NitrokeyManager> d_manager;
    bool d_device_connected;
    std::string d_device_serial;
    std::vector<uint8_t> d_key_data;
    size_t d_key_size;
    size_t d_output_index;
    bool d_auto_repeat;

public:
    nitrokey_interface(bool auto_repeat = false)
        : gr::block("nitrokey_interface",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(1, 1, sizeof(uint8_t))),
          d_device_connected(false),
          d_output_index(0),
          d_auto_repeat(auto_repeat)
    {
        d_manager = std::make_unique<NitrokeyManager>();
        connect_device();
    }

    ~nitrokey_interface() = default;

    void connect_device()
    {
        try {
            auto devices = d_manager->list_Devices();
            if (devices.empty()) {
                std::cerr << "No Nitrokey devices found" << std::endl;
                return;
            }

            // Connect to first available device
            d_manager->connect(devices[0]);
            d_device_connected = true;
            d_device_serial = devices[0];
            std::cout << "Connected to Nitrokey device: " << d_device_serial << std::endl;
        } catch (const DeviceNotConnectedException& e) {
            std::cerr << "Failed to connect to Nitrokey device: " << e.what() << std::endl;
            d_device_connected = false;
        } catch (const std::exception& e) {
            std::cerr << "Nitrokey connection error: " << e.what() << std::endl;
            d_device_connected = false;
        }
    }

    bool load_key_from_slot(int slot)
    {
        if (!d_device_connected) {
            std::cerr << "Device not connected" << std::endl;
            return false;
        }

        try {
            // Load key from Nitrokey slot
            auto key_data = d_manager->get_slot_data(slot);
            if (key_data.empty()) {
                std::cerr << "No key data in slot " << slot << std::endl;
                return false;
            }

            d_key_data = key_data;
            d_key_size = d_key_data.size();
            d_output_index = 0;
            return true;
        } catch (const CommandFailedException& e) {
            std::cerr << "Failed to load key from slot " << slot << ": " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error loading key: " << e.what() << std::endl;
            return false;
        }
    }

    bool store_key_to_slot(int slot, const std::vector<uint8_t>& key_data)
    {
        if (!d_device_connected) {
            std::cerr << "Device not connected" << std::endl;
            return false;
        }

        try {
            // Store key to Nitrokey slot
            d_manager->set_slot_data(slot, key_data);
            return true;
        } catch (const CommandFailedException& e) {
            std::cerr << "Failed to store key to slot " << slot << ": " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error storing key: " << e.what() << std::endl;
            return false;
        }
    }

    bool sign_data(const std::vector<uint8_t>& data, std::vector<uint8_t>& signature)
    {
        if (!d_device_connected) {
            std::cerr << "Device not connected" << std::endl;
            return false;
        }

        try {
            // Sign data using Nitrokey
            signature = d_manager->sign_data(data);
            return true;
        } catch (const CommandFailedException& e) {
            std::cerr << "Failed to sign data: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error signing data: " << e.what() << std::endl;
            return false;
        }
    }

    bool verify_signature(const std::vector<uint8_t>& data, 
                         const std::vector<uint8_t>& signature)
    {
        if (!d_device_connected) {
            std::cerr << "Device not connected" << std::endl;
            return false;
        }

        try {
            // Verify signature using Nitrokey
            return d_manager->verify_signature(data, signature);
        } catch (const CommandFailedException& e) {
            std::cerr << "Failed to verify signature: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error verifying signature: " << e.what() << std::endl;
            return false;
        }
    }

    int general_work(int noutput_items,
                    gr_vector_int& ninput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items) override
    {
        if (!d_device_connected || d_key_data.empty()) {
            return 0; // No data available
        }

        uint8_t* out = static_cast<uint8_t*>(output_items[0]);
        int produced = 0;

        for (int i = 0; i < noutput_items; i++) {
            if (d_output_index >= d_key_size) {
                if (d_auto_repeat) {
                    d_output_index = 0; // Reset to beginning
                } else {
                    break; // No more data
                }
            }

            out[i] = d_key_data[d_output_index];
            d_output_index++;
            produced++;
        }

        return produced;
    }

    // Public interface methods
    bool is_device_connected() const { return d_device_connected; }
    std::string get_device_serial() const { return d_device_serial; }
    bool is_key_loaded() const { return !d_key_data.empty(); }
    size_t get_key_size() const { return d_key_size; }
    
    void set_auto_repeat(bool repeat) { d_auto_repeat = repeat; }
    bool get_auto_repeat() const { return d_auto_repeat; }

    // Device management
    void disconnect_device()
    {
        if (d_device_connected) {
            d_manager->disconnect();
            d_device_connected = false;
            d_device_serial.clear();
        }
    }

    void reconnect_device() 
    {
        disconnect_device();
        connect_device();
    }

    // Key management
    void clear_key_data()
    {
        d_key_data.clear();
        d_key_size = 0;
        d_output_index = 0;
    }

    // Utility functions
    std::vector<std::string> list_available_devices()
    {
        try {
            return d_manager->list_Devices();
        } catch (const std::exception& e) {
            std::cerr << "Error listing devices: " << e.what() << std::endl;
            return {};
        }
    }

    bool is_device_available()
    {
        try {
            auto devices = d_manager->list_Devices();
            return !devices.empty();
        } catch (const std::exception& e) {
            return false;
        }
    }
};

// Factory function
gr::block::sptr make_nitrokey_interface(bool auto_repeat)
{
    return gr::block::sptr(new nitrokey_interface(auto_repeat));
}

} // namespace linux_crypto
} // namespace gr

