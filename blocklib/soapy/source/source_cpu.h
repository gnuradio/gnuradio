#pragma once

#include "block_impl.h"
#include <gnuradio/soapy/source.h>
namespace gr {
namespace soapy {

template <class T>
class source_cpu : public source<T>, public block_impl
{
public:
    source_cpu(const typename source<T>::block_args& args);

    work_return_code_t work(work_io&) override;

private:
};


} // namespace soapy
} // namespace gr
