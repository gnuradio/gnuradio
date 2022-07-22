#pragma once

#include "base.h"
#include <gnuradio/zeromq/push_sink.h>

namespace gr {
namespace zeromq {

class push_sink_cpu : public virtual push_sink, public virtual base_sink
{
public:
    push_sink_cpu(block_args args);
    work_return_code_t work(work_io&) override;
    std::string last_endpoint() const override { return base_sink::last_endpoint(); }

    // Since vsize can be set as 0, then inferred on flowgraph init, set it during start()
    bool start() override
    {
        set_vsize(this->input_stream_ports()[0]->itemsize());
        return push_sink::start();
    }
};

} // namespace zeromq
} // namespace gr