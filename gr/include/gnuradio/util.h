#pragma once

namespace gr {
static inline unsigned int round_down(unsigned int n, unsigned int multiple)
{
    return (n / multiple) * multiple;
}
} // namespace gr