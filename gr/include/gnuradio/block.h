#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <gnuradio/api.h>
#include <gnuradio/block_work_io.h>
#include <gnuradio/neighbor_interface.h>
#include <gnuradio/node.h>
#include <gnuradio/parameter.h>
#include <gnuradio/sptr_magic.h>

#include <pmtf/map.hpp>
#include <pmtf/string.hpp>
#include <pmtf/wrap.hpp>

namespace gr {

class pyblock_detail;
/**
 * @brief The abstract base class for all signal processing blocks in the GR
 * Block Library
 *
 * Blocks are the bare abstraction of an entity that has a name and a set of
 * inputs and outputs  These are never instantiated directly; rather, this is
 * the abstract parent class of blocks that implement actual signal processing
 * functions.
 *
 */
class GR_RUNTIME_API block : public gr::node
{
private:
    bool d_running = false;
    const std::string s_module;
    std::string d_suffix = "";
    tag_propagation_policy_t d_tag_propagation_policy;
    size_t d_output_multiple = 1;
    bool d_output_multiple_set = false;
    double d_relative_rate = 1.0;

protected:
    bool _is_hier = false;
    neighbor_interface_sptr p_scheduler = nullptr;
    std::map<std::string, int> d_param_str_map;
    std::map<int, std::string> d_str_param_map;
    std::shared_ptr<pyblock_detail> d_pyblock_detail;
    bool d_finished = false;

    void notify_scheduler();
    void notify_scheduler_input();
    void notify_scheduler_output();
    void come_back_later(size_t time_ms);
    std::atomic<bool> d_sleeping = false;

    work_io d_work_io;

public:
    /**
     * @brief Construct a new block object
     *
     * @param name The non-unique name of this block representing the block type
     */
    block(const std::string& name, const std::string& module = "");
    ~block() override{};
    virtual bool start();
    virtual bool stop();
    virtual bool done();

    void populate_work_io();
    work_io& get_work_io();

    bool finished() { return d_finished; }

    using sptr = std::shared_ptr<block>;
    sptr base() { return std::dynamic_pointer_cast<block>(shared_from_this()); }

    tag_propagation_policy_t tag_propagation_policy();
    void set_tag_propagation_policy(tag_propagation_policy_t policy);
    void set_pyblock_detail(std::shared_ptr<pyblock_detail> p);
    std::shared_ptr<pyblock_detail> pb_detail();
    /**
     * @brief Abstract method to call signal processing work from a derived block
     *
     * @param work_input Vector of block_work_input structs
     * @param work_output Vector of block_work_output structs
     * @return work_return_code_t
     */
    virtual work_return_code_t work(work_io& wio)
    {
        throw std::runtime_error("work function has been called but not implemented");
    }
    using work_t = std::function<work_return_code_t(work_io&)>;
    /**
     * @brief Wrapper for work to perform special checks and take care of special
     * cases for certain types of blocks, e.g. sync_block, decim_block
     *
     * @param work_input Vector of block_work_input structs
     * @param work_output Vector of block_work_output structs
     * @return work_return_code_t
     */
    virtual work_return_code_t do_work(work_io& wio) { return work(wio); };

    void set_parent_intf(neighbor_interface_sptr sched) { p_scheduler = sched; }
    parameter_config d_parameters;
    void add_param(const std::string& name, int id, pmt_sptr p)
    {
        d_parameters.add(name, id, p);
    }
    pmtf::pmt request_parameter_query(int param_id);
    pmtf::pmt request_parameter_query(const std::string& param_str)
    {
        return request_parameter_query(get_param_id(param_str));
    }
    void request_parameter_change(int param_id, pmtf::pmt new_value, bool block = true);
    void request_parameter_change(const std::string& param_str,
                                  pmtf::pmt new_value,
                                  bool block = true)
    {
        return request_parameter_change(get_param_id(param_str), new_value, block);
    };
    virtual void on_parameter_change(param_action_sptr action);
    virtual void on_parameter_query(param_action_sptr action);
    void set_output_multiple(size_t multiple);
    size_t output_multiple() const { return d_output_multiple; }
    bool output_multiple_set() const { return d_output_multiple_set; }
    void set_relative_rate(double relative_rate) { d_relative_rate = relative_rate; }
    double relative_rate() const { return d_relative_rate; }

    virtual int get_param_id(const std::string& id) { return d_param_str_map[id]; }
    virtual std::string get_param_str(const int id) { return d_str_param_map[id]; }
    virtual std::string suffix() { return ""; }
    std::string to_json();
    void from_json(const std::string& json_str);

    /**
     * Every Block should have a param update message handler
     */
    virtual void handle_msg_param_update(pmtf::pmt msg);

    virtual void handle_msg_system(pmtf::pmt msg);

    static pmtf::pmt deserialize_param_to_pmt(const std::string& param_value);
    static sptr cast(node_sptr n) { return std::static_pointer_cast<block>(n); }
    bool is_hier() { return _is_hier; }
};

using block_sptr = block::sptr;
using block_vector_t = std::vector<block_sptr>;

} // namespace gr
