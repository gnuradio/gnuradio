#include "buffer_cpu_vmcirc_mmap_shm_open.h"

#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <stdexcept>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include "pagesize.h"
#include <gnuradio/sys_paths.h>
#include <fmt/core.h>
#include <cerrno>
#include <cstdio>

namespace gr {
buffer_cpu_vmcirc_mmap_shm_open::buffer_cpu_vmcirc_mmap_shm_open(
    size_t num_items, size_t item_size, std::shared_ptr<buffer_properties> buf_properties)
    : buffer_cpu_vmcirc(num_items, item_size, gr::pagesize(), buf_properties)
{
    set_type("buffer_cpu_vmcirc_mmap_shm_open");

#if !defined(HAVE_MMAP) || !defined(HAVE_SHM_OPEN)
    d_logger->error("mmap or shm_open is not available");
    throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
#else
    std::scoped_lock guard(s_vm_mutex);

    static int s_seg_counter = 0;

    int pagesize = gr::pagesize();

    if (_buf_size <= 0 || (_buf_size % pagesize) != 0) {
        this->d_logger->error("invalid _buf_size = {}", _buf_size);
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }


    int shm_fd = -1;
    std::string seg_name;
    static bool portable_format = true;

    // open a new named shared memory segment
    while (1) {
        if (portable_format) {

            // This is the POSIX recommended "portable format".
            // Of course the "portable format" doesn't work on some systems...
            seg_name = fmt::format("/gnuradio-{}-{}", getpid(), s_seg_counter);
        }
        else {

            // Where the "portable format" doesn't work, we try building
            // a full filesystem pathname pointing into a suitable temporary directory.

            seg_name =
                fmt::format("{}/gnuradio-{}-{}", gr::tmp_path(), getpid(), s_seg_counter);
        }

        shm_fd = shm_open(seg_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
        if (shm_fd == -1 && errno == EACCES && portable_format) {
            portable_format = false;
            continue; // try again using "non-portable format"
        }

        s_seg_counter++;

        if (shm_fd == -1) {
            if (errno ==
                EEXIST) // Named segment already exists (shouldn't happen).  Try again
                continue;

            d_logger->error("shm_open [{}] failed", seg_name);
            throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
        }
        break;
    }

    // We've got a new shared memory segment fd open.
    // Now set it's length to 2x what we really want and mmap it in.
    if (ftruncate(shm_fd, (off_t)2 * _buf_size) == -1) {
        close(shm_fd); // cleanup
        d_logger->error("ftruncate failed");
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }

    void* first_copy =
        mmap(0, 2 * _buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, (off_t)0);

    if (first_copy == MAP_FAILED) {
        close(shm_fd); // cleanup
        d_logger->error("mmap (1) failed");
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }

    // unmap the 2nd half
    if (munmap((char*)first_copy + _buf_size, _buf_size) == -1) {
        close(shm_fd); // cleanup
        d_logger->error("munmap (1) failed");
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }

    // map the first half into the now available hole where the
    // second half used to be.
    void* second_copy = mmap((char*)first_copy + _buf_size,
                             _buf_size,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED,
                             shm_fd,
                             (off_t)0);

    if (second_copy == MAP_FAILED) {
        close(shm_fd); // cleanup
        d_logger->error("mmap (2) failed");
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }

#if 0 // OS/X doesn't allow you to resize the segment

    // cut the shared memory segment down to size
    if(ftruncate(shm_fd, (off_t)size) == -1) {
      close(shm_fd);						// cleanup
      perror("gr::buffer_cpu_vmcirc_mmap_shm_open: ftruncate (2)");
      throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }
#endif

    close(shm_fd); // fd no longer needed.  The mapping is retained.

    if (shm_unlink(seg_name.c_str()) == -1) { // unlink the seg_name.
        d_logger->error("shm_unlink failed");
        throw std::runtime_error("gr::buffer_cpu_vmcirc_mmap_shm_open");
    }

    // Now remember the important stuff
    _buffer = (uint8_t*)first_copy;
    d_size = _buf_size;
#endif
}

buffer_cpu_vmcirc_mmap_shm_open::~buffer_cpu_vmcirc_mmap_shm_open()
{
#if defined(HAVE_MMAP)
    std::scoped_lock guard(s_vm_mutex);

    if (munmap(d_base, 2 * d_size) == -1) {
        d_logger->error("munmap (2) failed");
    }
#endif
}

} // namespace gr
