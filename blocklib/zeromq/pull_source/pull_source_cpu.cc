#include "pull_source_cpu.h"
#include "pull_source_cpu_gen.h"

#include <chrono>
#include <thread>

namespace gr {
namespace zeromq {

pull_source_cpu::pull_source_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      base_source(
          ZMQ_PULL, args.itemsize, args.address, args.timeout, args.pass_tags, args.hwm)
{
}

work_return_code_t pull_source_cpu::work(work_io& wio)
{

    auto noutput_items = wio.outputs()[0].n_items;
    bool first = true;
    size_t done = 0;

    /* Process as much as we can */
    while (1) {
        if (has_pending()) {
            /* Flush anything pending */
            done += flush_pending(wio.outputs()[0], noutput_items - done, done);

            /* No more space ? */
            if (done == noutput_items)
                break;
        }
        else {
            /* Try to get the next message */
            if (!load_message(first)) {
                // Launch a thread to come back and try again some time later
                come_back_later(100);
                break; /* No message, we're done for now */
            }

            /* Not the first anymore */
            first = false;
        }
    }

    wio.outputs()[0].n_produced = done;
    return work_return_code_t::WORK_OK;
}


} // namespace zeromq
} // namespace gr
