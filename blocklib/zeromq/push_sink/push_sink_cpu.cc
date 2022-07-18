#include "push_sink_cpu.h"
#include "push_sink_cpu_gen.h"

namespace gr {
namespace zeromq {

push_sink_cpu::push_sink_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      base_sink(
          ZMQ_PUSH, args.itemsize, args.address, args.timeout, args.pass_tags, args.hwm)
{
}
work_return_code_t push_sink_cpu::work(work_io& wio)

{
    // Poll with a timeout (FIXME: scheduler can't wait for us)
    zmq::pollitem_t itemsout[] = { { static_cast<void*>(d_socket), 0, ZMQ_POLLOUT, 0 } };
    zmq::poll(&itemsout[0], 1, std::chrono::milliseconds{ d_timeout });

    // If we can send something, do it
    if (itemsout[0].revents & ZMQ_POLLOUT) {
        wio.inputs()[0].n_consumed =
            send_message(wio.inputs()[0].raw_items(),
                         wio.inputs()[0].n_items,
                         wio.inputs()[0].nitems_read(),
                         wio.inputs()[0].tags_in_window(0, wio.inputs()[0].n_items));
    }

    return work_return_code_t::WORK_OK;
}


} // namespace zeromq
} // namespace gr