#pragma once

#include <gnuradio/analog/agc.h>
#include <gnuradio/kernel/analog/agc.h>

namespace gr {
namespace analog {

template <class T>
class agc_cpu : public agc<T>, kernel::analog::agc<T>
{
public:
    agc_cpu(const typename agc<T>::block_args& args);
    work_return_code_t work(work_io&) override;

protected:
};

} // namespace analog
} // namespace gr
