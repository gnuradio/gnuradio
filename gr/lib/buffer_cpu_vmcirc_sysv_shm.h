#pragma once

#include <gnuradio/buffer_cpu_vmcirc.h>

namespace gr {
class buffer_cpu_vmcirc_sysv_shm : public buffer_cpu_vmcirc
{

public:
    using sptr = std::shared_ptr<buffer_cpu_vmcirc>;
    buffer_cpu_vmcirc_sysv_shm(size_t num_items,
                               size_t item_size,
                               std::shared_ptr<buffer_properties> buf_properties);
    ~buffer_cpu_vmcirc_sysv_shm() override;
};

} // namespace gr
