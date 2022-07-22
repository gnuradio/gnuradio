#pragma once

#include <gnuradio/buffer_cpu_vmcirc.h>

namespace gr {
class buffer_cpu_vmcirc_mmap_shm_open : public buffer_cpu_vmcirc
{
private:
    int d_size;
    char* d_base;

public:
    using sptr = std::shared_ptr<buffer_cpu_vmcirc>;
    buffer_cpu_vmcirc_mmap_shm_open(size_t num_items,
                                    size_t item_size,
                                    std::shared_ptr<buffer_properties> buf_properties);
    ~buffer_cpu_vmcirc_mmap_shm_open() override;
};

} // namespace gr
