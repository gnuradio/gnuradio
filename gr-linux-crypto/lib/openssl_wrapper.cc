#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>

namespace gr {
namespace linux_crypto {

class openssl_wrapper : public gr::block
{
private:
    EVP_CIPHER_CTX* d_encrypt_ctx;
    EVP_CIPHER_CTX* d_decrypt_ctx;
    std::vector<uint8_t> d_key;
    std::vector<uint8_t> d_iv;
    const EVP_CIPHER* d_cipher;
    bool d_encrypt;
    std::vector<uint8_t> d_buffer;
    size_t d_buffer_pos;
    bool d_initialized;

public:
    openssl_wrapper(const std::vector<uint8_t>& key,
                   const std::vector<uint8_t>& iv,
                   const std::string& algorithm = "aes-256-cbc",
                   bool encrypt = true)
        : gr::block("openssl_wrapper",
                    gr::io_signature::make(1, 1, sizeof(uint8_t)),
                    gr::io_signature::make(1, 1, sizeof(uint8_t))),
          d_key(key),
          d_iv(iv),
          d_encrypt(encrypt),
          d_buffer_pos(0),
          d_initialized(false)
    {
        d_encrypt_ctx = EVP_CIPHER_CTX_new();
        d_decrypt_ctx = EVP_CIPHER_CTX_new();
        
        if (!d_encrypt_ctx || !d_decrypt_ctx) {
            std::cerr << "Failed to create OpenSSL contexts" << std::endl;
            return;
        }

        set_algorithm(algorithm);
        d_buffer.resize(EVP_CIPHER_block_size(d_cipher));
    }

    ~openssl_wrapper()
    {
        if (d_encrypt_ctx) {
            EVP_CIPHER_CTX_free(d_encrypt_ctx);
        }
        if (d_decrypt_ctx) {
            EVP_CIPHER_CTX_free(d_decrypt_ctx);
        }
    }

    void set_algorithm(const std::string& algorithm)
    {
        if (algorithm == "aes-128-cbc") {
            d_cipher = EVP_aes_128_cbc();
        } else if (algorithm == "aes-192-cbc") {
            d_cipher = EVP_aes_192_cbc();
        } else if (algorithm == "aes-256-cbc") {
            d_cipher = EVP_aes_256_cbc();
        } else if (algorithm == "aes-128-ecb") {
            d_cipher = EVP_aes_128_ecb();
        } else if (algorithm == "aes-256-ecb") {
            d_cipher = EVP_aes_256_ecb();
        } else {
            std::cerr << "Unsupported algorithm: " << algorithm << std::endl;
            d_cipher = EVP_aes_256_cbc(); // Default
        }

        initialize_contexts();
    }

    void initialize_contexts()
    {
        // Initialize encryption context
        if (EVP_EncryptInit_ex(d_encrypt_ctx, d_cipher, NULL, d_key.data(), d_iv.data()) != 1) {
            std::cerr << "Failed to initialize encryption context" << std::endl;
            print_openssl_error();
            return;
        }

        // Initialize decryption context
        if (EVP_DecryptInit_ex(d_decrypt_ctx, d_cipher, NULL, d_key.data(), d_iv.data()) != 1) {
            std::cerr << "Failed to initialize decryption context" << std::endl;
            print_openssl_error();
            return;
        }

        d_initialized = true;
    }

    int general_work(int noutput_items,
                    gr_vector_int& ninput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items) override
    {
        if (!d_initialized) {
            return 0;
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
            if (d_buffer_pos == d_buffer.size()) {
                int len = 0;
                if (d_encrypt) {
                    if (EVP_EncryptUpdate(d_encrypt_ctx, out + produced, &len, 
                                        d_buffer.data(), d_buffer_pos) != 1) {
                        std::cerr << "Encryption failed" << std::endl;
                        print_openssl_error();
                        break;
                    }
                } else {
                    if (EVP_DecryptUpdate(d_decrypt_ctx, out + produced, &len,
                                        d_buffer.data(), d_buffer_pos) != 1) {
                        std::cerr << "Decryption failed" << std::endl;
                        print_openssl_error();
                        break;
                    }
                }
                produced += len;
                d_buffer_pos = 0;
            }
        }

        consume_each(consumed);
        return produced;
    }

    // Finalize processing (call this at the end of stream)
    int finalize(uint8_t* output, int max_output_len)
    {
        if (!d_initialized) {
            return 0;
        }

        int len = 0;
        int final_len = 0;

        if (d_encrypt) {
            if (EVP_EncryptFinal_ex(d_encrypt_ctx, output, &final_len) != 1) {
                std::cerr << "Encryption finalization failed" << std::endl;
                print_openssl_error();
                return 0;
            }
        } else {
            if (EVP_DecryptFinal_ex(d_decrypt_ctx, output, &final_len) != 1) {
                std::cerr << "Decryption finalization failed" << std::endl;
                print_openssl_error();
                return 0;
            }
        }

        return final_len;
    }

    // Utility functions
    static std::vector<uint8_t> generate_random_key(int key_size)
    {
        std::vector<uint8_t> key(key_size);
        if (RAND_bytes(key.data(), key_size) != 1) {
            std::cerr << "Failed to generate random key" << std::endl;
            return {};
        }
        return key;
    }

    static std::vector<uint8_t> generate_random_iv(int iv_size)
    {
        std::vector<uint8_t> iv(iv_size);
        if (RAND_bytes(iv.data(), iv_size) != 1) {
            std::cerr << "Failed to generate random IV" << std::endl;
            return {};
        }
        return iv;
    }

    static std::vector<uint8_t> hash_data(const std::vector<uint8_t>& data, 
                                         const std::string& algorithm = "sha256")
    {
        const EVP_MD* md = nullptr;
        if (algorithm == "sha1") {
            md = EVP_sha1();
        } else if (algorithm == "sha256") {
            md = EVP_sha256();
        } else if (algorithm == "sha512") {
            md = EVP_sha512();
        } else {
            std::cerr << "Unsupported hash algorithm: " << algorithm << std::endl;
            return {};
        }

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) {
            return {};
        }

        if (EVP_DigestInit_ex(ctx, md, NULL) != 1) {
            EVP_MD_CTX_free(ctx);
            return {};
        }

        if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1) {
            EVP_MD_CTX_free(ctx);
            return {};
        }

        std::vector<uint8_t> hash(EVP_MD_size(md));
        unsigned int hash_len;
        if (EVP_DigestFinal_ex(ctx, hash.data(), &hash_len) != 1) {
            EVP_MD_CTX_free(ctx);
            return {};
        }

        EVP_MD_CTX_free(ctx);
        return hash;
    }

private:
    void print_openssl_error()
    {
        unsigned long err = ERR_get_error();
        if (err != 0) {
            char err_buf[256];
            ERR_error_string_n(err, err_buf, sizeof(err_buf));
            std::cerr << "OpenSSL error: " << err_buf << std::endl;
        }
    }

public:
    // Public interface methods
    bool is_initialized() const { return d_initialized; }
    bool is_encrypt() const { return d_encrypt; }
    void set_encrypt(bool encrypt) { d_encrypt = encrypt; }
    void set_key(const std::vector<uint8_t>& key) { d_key = key; initialize_contexts(); }
    void set_iv(const std::vector<uint8_t>& iv) { d_iv = iv; initialize_contexts(); }
};

// Factory function
gr::block::sptr make_openssl_wrapper(const std::vector<uint8_t>& key,
                                    const std::vector<uint8_t>& iv,
                                    const std::string& algorithm,
                                    bool encrypt)
{
    return gr::block::sptr(new openssl_wrapper(key, iv, algorithm, encrypt));
}

} // namespace linux_crypto
} // namespace gr

