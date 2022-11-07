#include <gnuradio/block_group_properties.h>
#include <gnuradio/buffer_cpu_host.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
// #include <gnuradio/buffer_cpu_simple.h>
#include <gnuradio/graph_utils.h>
#include <gnuradio/scheduler.h>
#include <gnuradio/thread_wrapper.h>

#include "scheduler_nbt_options.h"

namespace gr {
namespace schedulers {

class scheduler_nbt : public scheduler
{
private:
    std::vector<thread_wrapper::sptr> _threads;
    scheduler_options_sptr _opts;
    std::map<nodeid_t, neighbor_interface_sptr> _block_thread_map;
    std::vector<block_group_properties> _block_groups;

public:
    using sptr = std::shared_ptr<scheduler_nbt>;
    static sptr make(const std::string& opts)
    {
        return std::make_shared<scheduler_nbt>(
            scheduler_nbt_options::opts_from_yaml(opts));
    }
    static sptr
    make(scheduler_nbt_options_sptr opts = scheduler_nbt_options::opts_from_yaml("{}"))
    {
        return std::make_shared<scheduler_nbt>(opts);
    }
    scheduler_nbt(scheduler_nbt_options_sptr opts) : scheduler(opts->name), _opts(opts)
    {
        if (opts->default_buffer_type == "cpu_host") {
            _default_buf_properties =
                buffer_cpu_host_properties::make(buffer_cpu_host_type::H2H);
        }
        else {
            _default_buf_properties =
                buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO);
        }
    }
    ~scheduler_nbt() override{};

    void push_message(scheduler_message_sptr msg) override;
    void add_block_group(const std::vector<block_sptr>& blocks,
                         const std::string& name = "",
                         const std::vector<unsigned int>& affinity_mask = {});

    /**
     * @brief Initialize the multi-threaded scheduler
     *
     * Creates a single-threaded scheduler for each block group, then for each block that
     * is not part of a block group
     *
     * @param fg subgraph assigned to this multi-threaded scheduler
     * @param fgmon sptr to flowgraph monitor object
     * @param block_sched_map for each block in this flowgraph, a map of neighboring
     * schedulers
     */
    void initialize(flat_graph_sptr fg, runtime_monitor_sptr fgmon) override;
    void start() override;
    void stop() override;
    void wait() override;
    void run();
    void kill() override;
};
} // namespace schedulers
} // namespace gr
