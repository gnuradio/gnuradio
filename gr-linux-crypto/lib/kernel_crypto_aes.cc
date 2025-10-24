#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/vector_source.h>
#include <linux/if_alg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>

namespace gr {
namespace linux_crypto {

class kernel_crypto_aes : public gr::block
{
private:
    int d_socket;
    int d_tfm;
    std::vector<uint8_t> d_key;
    std::vector<uint8_t> d_iv;
    size_t d_block_size;
    bool d_encrypt;
    std::vector<uint8_t> d_buffer;
    size_t d_buffer_pos;

public:
    kernel_crypto_aes(const std::vector<uint8_t>& key, 
                      const std::vector<uint8_t>& iv,
                      bool encrypt = true)
        : gr::block("kernel_crypto_aes",
                    gr::io_signature::make(1, 1, sizeof(uint8_t)),
                    gr::io_signature::make(1, 1, sizeof(uint8_t))),
          d_key(key),
          d_iv(iv),
          d_block_size(16), // AES block size
          d_encrypt(encrypt),
          d_buffer_pos(0)
    {
        d_buffer.resize(d_block_size);
        setup_kernel_crypto();
    }

    ~kernel_crypto_aes()
    {
        if (d_socket >= 0) {
            close(d_socket);
        }
    }

    void setup_kernel_crypto()
    {
        // Create AF_ALG socket
        d_socket = socket(AF_ALG, SOCK_SEQPACKET, 0);
        if (d_socket < 0) {
            std::cerr << "Failed to create AF_ALG socket: " << strerror(errno) << std::endl;
            return;
        }

        // Set algorithm
        struct sockaddr_alg sa = {};
        sa.salg_family = AF_ALG;
        strcpy(sa.salg_type, "skcipher");
        strcpy(sa.salg_name, "cbc(aes)");

        if (bind(d_socket, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
            std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
            close(d_socket);
            d_socket = -1;
            return;
        }

        // Set key
        if (setsockopt(d_socket, SOL_ALG, ALG_SET_KEY, d_key.data(), d_key.size()) < 0) {
            std::cerr << "Failed to set key: " << strerror(errno) << std::endl;
            close(d_socket);
            d_socket = -1;
            return;
        }

        // Accept connection
        d_tfm = accept(d_socket, NULL, NULL);
        if (d_tfm < 0) {
            std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
            close(d_socket);
            d_socket = -1;
            return;
        }
    }

    int general_work(int noutput_items,
                    gr_vector_int& ninput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items) override
    {
        if (d_socket < 0) {
            return 0; // Crypto not available
        }

        const uint8_t* in = static_cast<const uint8_t*>(input_items[0]);
        uint8_t* out = static_cast<uint8_t*>(output_items[0]);
        int consumed = 0;
        int produced = 0;

        for (int i = 0; i < ninput_items[0] && produced < noutput_items; i++) {
            d_buffer[d_buffer_pos] = in[i];
            d_buffer_pos++;
            consumed++;

            // Process when buffer is full
            if (d_buffer_pos == d_block_size) {
                if (process_block(d_buffer.data(), out + produced)) {
                    produced += d_block_size;
                }
                d_buffer_pos = 0;
            }
        }

        consume_each(consumed);
        return produced;
    }

private:
    bool process_block(const uint8_t* input, uint8_t* output)
    {
        struct msghdr msg = {};
        struct iovec iov = {};
        char cbuf[CMSG_SPACE(sizeof(struct af_alg_iv))];
        struct cmsghdr* cmsg;
        struct af_alg_iv* iv;

        // Set up message
        iov.iov_base = const_cast<uint8_t*>(input);
        iov.iov_len = d_block_size;

        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cbuf;
        msg.msg_controllen = sizeof(cbuf);

        // Set IV
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_ALG;
        cmsg->cmsg_type = ALG_SET_IV;
        cmsg->cmsg_len = CMSG_LEN(sizeof(struct af_alg_iv));
        iv = (struct af_alg_iv*)CMSG_DATA(cmsg);
        iv->ivlen = d_iv.size();
        memcpy(iv->iv, d_iv.data(), d_iv.size());

        // Send data
        if (sendmsg(d_tfm, &msg, 0) < 0) {
            std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
            return false;
        }

        // Receive result
        if (recv(d_tfm, output, d_block_size, 0) < 0) {
            std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

public:
    // Public interface methods
    bool is_crypto_available() const { return d_socket >= 0; }
    size_t get_block_size() const { return d_block_size; }
    bool is_encrypt() const { return d_encrypt; }
    
    void set_encrypt(bool encrypt) { d_encrypt = encrypt; }
    void set_key(const std::vector<uint8_t>& key) { d_key = key; }
    void set_iv(const std::vector<uint8_t>& iv) { d_iv = iv; }
};

// Factory function
gr::block::sptr make_kernel_crypto_aes(const std::vector<uint8_t>& key,
                                      const std::vector<uint8_t>& iv,
                                      bool encrypt)
{
    return gr::block::sptr(new kernel_crypto_aes(key, iv, encrypt));
}

} // namespace linux_crypto
} // namespace gr

