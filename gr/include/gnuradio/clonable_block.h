#pragma once

#include <gnuradio/block.h>

namespace gr {


/**
 * @brief clonable interface to block
 *
 */
class clonable_block
{
public:
    virtual ~clonable_block() {}
    virtual std::shared_ptr<block> clone() const = 0;
};
} // namespace gr
