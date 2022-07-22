#pragma once

#include <pmtf/base.hpp>
#include <string>

namespace gr {

/**
 * @brief Required methods for base block class with any RPC client
 *
 * The RPC client interface that a block can store if assigned allows
 * the used instantiation of the block to live on a remote host with
 * the original block object residing locally and able to have its
 * normal methods called
 *
 */
class pyblock_detail;

class rpc_client_interface
{
public:
    rpc_client_interface() {}
    virtual ~rpc_client_interface() {}
    virtual void block_method(const std::string& block_name,
                              const std::string& method,
                              const std::string& payload);

    virtual std::string block_parameter_query(const std::string& block_name,
                                              const std::string& parameter);

    virtual void block_parameter_change(const std::string& block_name,
                                        const std::string& parameter,
                                        const std::string& encoded_value);

    void set_pyblock_detail(std::shared_ptr<pyblock_detail> p);
    std::shared_ptr<pyblock_detail> pb_detail();

private:
    std::shared_ptr<pyblock_detail> d_pyblock_detail;
};

using rpc_client_interface_sptr = std::shared_ptr<rpc_client_interface>;


} // namespace gr