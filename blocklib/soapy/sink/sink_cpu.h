#pragma once

#include "block_impl.h"
#include <gnuradio/soapy/sink.h>
namespace gr {
namespace soapy {

template <class T>
class sink_cpu : public sink<T>, public block_impl
{
public:
    sink_cpu(const typename sink<T>::block_args& args);

    work_return_t work(work_io&) override;

private:
    size_t d_burst_remaining = 0;
};


} // namespace soapy
} // namespace gr
