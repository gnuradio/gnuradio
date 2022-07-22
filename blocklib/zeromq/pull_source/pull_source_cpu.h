#pragma once

#include "base.h"
#include <gnuradio/zeromq/pull_source.h>

namespace gr {
namespace zeromq {

class pull_source_cpu : public virtual pull_source, public virtual base_source
{
public:
    pull_source_cpu(block_args args);
    work_return_code_t work(work_io&) override;

    // Since vsize can be set as 0, then inferred on flowgraph init, set it during start()
    bool start() override
    {
        set_vsize(this->output_stream_ports()[0]->itemsize());
        return pull_source::start();
    }
};

} // namespace zeromq
} // namespace gr