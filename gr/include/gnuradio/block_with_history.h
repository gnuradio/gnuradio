#pragma once

#include <algorithm>
#include <cstddef>

namespace gr {
class block_with_history
{
protected:
    size_t _noconsume = 0;

public:
    void declare_noconsume(size_t value) { _noconsume = value; }
    size_t noconsume() { return _noconsume; }

    size_t calc_upstream_buffer_with_history(size_t nitems,
                                             double relative_rate,
                                             size_t output_multiple)
    {
        double decimation = (1.0 / relative_rate);
        int multiple = output_multiple;
        return std::max(nitems,
                        static_cast<size_t>(2 * (decimation * multiple + noconsume())));
    }
};
} // namespace gr