#include "buffer_cpu_vmcirc_sysv_shm.h"

#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include "pagesize.h"
#include <gnuradio/logger.h>
#include <cerrno>
#include <cstdio>

#define MAX_SYSV_SHM_ATTEMPTS 3

namespace gr {
buffer_cpu_vmcirc_sysv_shm::buffer_cpu_vmcirc_sysv_shm(
    size_t num_items, size_t item_size, std::shared_ptr<buffer_properties> buf_properties)
    : buffer_cpu_vmcirc(num_items, item_size, gr::pagesize(), buf_properties)
{
    set_type("buffer_cpu_vmcirc_sysv_shm");

#if !defined(HAVE_SYS_SHM_H)
    d_logger->error("sysv shared memory is not available");
    throw std::runtime_error("gr::buffer_cpu_vmcirc_sysv_shm");
#else

    std::scoped_lock guard(s_vm_mutex);

    int pagesize = gr::pagesize();

    if (_buf_size <= 0 || (_buf_size % pagesize) != 0) {
        d_logger->error("invalid _buf_size = {}", _buf_size);
        throw std::runtime_error("gr::buffer_cpu_vmcirc_sysv_shm");
    }

    // Attempt to allocate buffers (handle bad_alloc errors)
    int attempts_remain(MAX_SYSV_SHM_ATTEMPTS);
    while (attempts_remain-- > 0) {

        int shmid_guard = -1;
        int shmid1 = -1;
        int shmid2 = -1;

        // We use this as a guard page.  We'll map it read-only on both ends of the
        // buffer. Ideally we'd map it no access, but I don't think that's possible with
        // SysV
        if ((shmid_guard = shmget(IPC_PRIVATE, pagesize, IPC_CREAT | 0400)) == -1) {
            d_logger->error("shmget (0): %s", strerror(errno));
            continue;
        }

        if ((shmid2 = shmget(
                 IPC_PRIVATE, 2 * _buf_size + 2 * pagesize, IPC_CREAT | 0700)) == -1) {
            d_logger->error("shmget (1): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            continue;
        }

        if ((shmid1 = shmget(IPC_PRIVATE, _buf_size, IPC_CREAT | 0700)) == -1) {
            d_logger->error("shmget (2): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid2, IPC_RMID, 0);
            continue;
        }

        void* first_copy = shmat(shmid2, 0, 0);
        if (first_copy == (void*)-1) {
            d_logger->error("shmat (1): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid2, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            continue;
        }

        shmctl(shmid2, IPC_RMID, 0);

        // There may be a race between our detach and attach.
        //
        // If the system allocates all shared memory segments at the same
        // virtual addresses in all processes and if the system allocates
        // some other segment to first_copy or first_copoy + _buf_size between
        // our detach and attach, the attaches below could fail [I've never
        // seen it fail for this reason].
        shmdt(first_copy);

        // first read-only guard page
        if (shmat(shmid_guard, first_copy, SHM_RDONLY) == (void*)-1) {
            d_logger->error("shmat (2): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            continue;
        }

        // first copy
        if (shmat(shmid1, (uint8_t*)first_copy + pagesize, 0) == (void*)-1) {
            d_logger->error("shmat (3): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt(first_copy);
            continue;
        }

        // second copy
        if (shmat(shmid1, (uint8_t*)first_copy + pagesize + _buf_size, 0) == (void*)-1) {
            d_logger->error("shmat (4): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt((uint8_t*)first_copy + pagesize);
            continue;
        }

        // second read-only guard page
        if (shmat(shmid_guard,
                  (uint8_t*)first_copy + pagesize + 2 * _buf_size,
                  SHM_RDONLY) == (void*)-1) {
            d_logger->error("shmat (5): %s", strerror(errno));
            shmctl(shmid_guard, IPC_RMID, 0);
            shmctl(shmid1, IPC_RMID, 0);
            shmdt(first_copy);
            shmdt((uint8_t*)first_copy + pagesize);
            shmdt((uint8_t*)first_copy + pagesize + _buf_size);
            continue;
        }

        shmctl(shmid1, IPC_RMID, 0);
        shmctl(shmid_guard, IPC_RMID, 0);

        // Now remember the important stuff
        _buffer = (uint8_t*)first_copy + pagesize;

        break;
    }
    if (attempts_remain < 0) {
        throw std::runtime_error("gr::buffer_cpu_vmcirc_sysv_shm");
    }
#endif
}

buffer_cpu_vmcirc_sysv_shm::~buffer_cpu_vmcirc_sysv_shm()
{
#if defined(HAVE_SYS_SHM_H)
    std::scoped_lock guard(s_vm_mutex);

    if (shmdt(_buffer - gr::pagesize()) == -1 || shmdt(_buffer) == -1 ||
        shmdt(_buffer + _buf_size) == -1 || shmdt(_buffer + 2 * _buf_size) == -1) {
        d_logger->error("shmdt (2): %s", strerror(errno));
    }
#endif
}

} // namespace gr
