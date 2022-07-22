#pragma once

#include <gnuradio/api.h>
#include <gnuradio/flat_graph.h>
#include <gnuradio/logger.h>
#include <gnuradio/neighbor_interface.h>

namespace gr {

class GR_RUNTIME_API buffer_manager
{
private:
    const int s_fixed_buf_size;
    static const int s_min_items_to_process = 1;
    const size_t s_min_buf_items = 1;

    std::string _name = "buffer_manager";
    logger_ptr d_logger;
    logger_ptr d_debug_logger;

public:
    using sptr = std::shared_ptr<buffer_manager>;
    buffer_manager(const unsigned int default_buffer_size_in_bytes)
        : s_fixed_buf_size(default_buffer_size_in_bytes)
    {
        gr::configure_default_loggers(d_logger, d_debug_logger, _name);
    }
    ~buffer_manager() {}

    void initialize_buffers(flat_graph_sptr fg,
                            std::shared_ptr<buffer_properties> buf_props,
                            neighbor_interface_sptr sched_intf = nullptr);

private:
    int get_buffer_num_items(edge_sptr e, flat_graph_sptr fg);
};

} // namespace gr
