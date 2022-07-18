#pragma once
#include <gnuradio/scheduler_message.h>

namespace gr {


class port_interface
{
public:
    virtual void push_message(scheduler_message_sptr msg) = 0;
    virtual ~port_interface() = default;
};

using port_interface_ptr = port_interface*;

} // namespace gr
