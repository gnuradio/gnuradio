#include "agc_cpu.h"
#include "agc_cpu_gen.h"

namespace gr {
namespace analog {

template <class T>
agc_cpu<T>::agc_cpu(const typename agc<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      kernel::analog::agc<T>(args.rate, args.reference, args.gain, 65536)
{
}

template <class T>
work_return_code_t agc_cpu<T>::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;
    kernel::analog::agc<T>::scaleN(out, in, noutput_items);

    wio.outputs()[0].n_produced = noutput_items;
    return work_return_code_t::WORK_OK;
}

} // namespace analog
} // namespace gr
