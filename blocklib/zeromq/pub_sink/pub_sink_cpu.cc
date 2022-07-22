#include "pub_sink_cpu.h"
#include "pub_sink_cpu_gen.h"

namespace gr {
namespace zeromq {

pub_sink_cpu::pub_sink_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      base_sink(ZMQ_PUB,
                args.itemsize,
                args.address,
                args.timeout,
                args.pass_tags,
                args.hwm,
                args.key)
{
}

work_return_code_t pub_sink_cpu::work(work_io& wio)
{
    auto noutput_items = wio.inputs()[0].n_items;
    auto nread = wio.inputs()[0].nitems_read();
    auto nsent = send_message(wio.inputs()[0].raw_items(),
                              noutput_items,
                              nread,
                              wio.inputs()[0].tags_in_window(0, noutput_items));
    wio.consume_each(nsent);
    return work_return_code_t::WORK_OK;
}


} // namespace zeromq
} // namespace gr
