#pragma once

#include <gnuradio/scheduler_message.h>

namespace gr {

struct neighbor_interface {
    neighbor_interface() {}
    virtual ~neighbor_interface() {}
    virtual void push_message(scheduler_message_sptr msg) = 0;
};
using neighbor_interface_sptr = std::shared_ptr<neighbor_interface>;

} // namespace gr
