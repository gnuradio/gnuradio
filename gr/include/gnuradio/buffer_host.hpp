#ifndef GNURADIO_BUFFER_HOST_HPP
#define GNURADIO_BUFFER_HOST_HPP

#include <memory_resource>
#include <algorithm>
#include <cassert> // to assert if compiled for debugging
#include <functional>
#include <numeric>
#include <ranges>
#include <span>

#include <fmt/format.h>

// header for creating/opening or POSIX shared memory objects
#include <errno.h>
#include <fcntl.h>
#if defined __has_include
#if __has_include(<sys/mman.h>) && __has_include(<sys/stat.h>) && __has_include(<unistd.h>)
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
namespace gr {
static constexpr bool has_posix_mmap_interface = true;
}
#define HAS_POSIX_MAP_INTERFACE
#elif
namespace gr {
static constexpr bool has_posix_mmap_interface = false;
}
#endif
#elif // #if defined __has_include -- required for portability
namespace gr {
static constexpr bool has_posix_mmap_interface = false;
}
#endif

#include "buffer.hpp"
#include "claim_strategy.hpp"
#include "sequence.hpp"
#include "wait_strategy.hpp"

namespace gr {

namespace util {
constexpr int round_up(int num_to_round, int multiple) noexcept
{
    if (multiple == 0) {
        return num_to_round;
    }
    const int remainder = num_to_round % multiple;
    if (remainder == 0) {
        return num_to_round;
    }
    return num_to_round + multiple - remainder;
}
} // namespace util

// clang-format off
class double_mapped_memory_resource : public std::pmr::memory_resource {
    [[nodiscard]] void* do_allocate(const std::size_t required_size, std::size_t alignment) override {
#ifdef HAS_POSIX_MAP_INTERFACE
        const std::size_t size = 2 * required_size;
        if (size % 2 != 0 || size % getpagesize() != 0) {
            throw std::runtime_error(fmt::format("incompatible buffer-byte-size: {} -> {} alignment: {} vs. page size: {}", required_size, size, alignment, getpagesize()));
        }
        const std::size_t size_half = size/2;

        static int _counter;
        const auto buffer_name = fmt::format("/double_mapped_memory_resource-{}-{}-{}", getpid(), size, _counter++);
        const auto memfd_create = [name = buffer_name.c_str()](unsigned int flags) -> int {
            return syscall(__NR_memfd_create, name, flags);
        };
        int shm_fd = memfd_create(0);
        if (shm_fd < 0) {
            throw std::runtime_error(fmt::format("{} - memfd_create error {}: {}",  buffer_name, errno, strerror(errno)));
        }

        if (ftruncate(shm_fd, static_cast<off_t>(size)) == -1) {
            close(shm_fd);
            throw std::runtime_error(fmt::format("{} - ftruncate {}: {}",  buffer_name, errno, strerror(errno)));
        }

        void* first_copy = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, static_cast<off_t>(0));
        if (first_copy == MAP_FAILED) {
            close(shm_fd);
            throw std::runtime_error(fmt::format("{} - failed munmap for first half {}: {}",  buffer_name, errno, strerror(errno)));
        }

        // unmap the 2nd half
        if (munmap((char*)first_copy + size_half, size_half) == -1) {
            close(shm_fd);
            throw std::runtime_error(fmt::format("{} - failed munmap for second half {}: {}",  buffer_name, errno, strerror(errno)));
        }

        // map the first half into the now available hole where the
        void* second_copy = mmap((char*)first_copy + size_half, size_half, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, (off_t)0);
        if (second_copy == MAP_FAILED) {
            close(shm_fd);
            throw std::runtime_error(fmt::format("{} - failed mmap for second copy {}: {}",  buffer_name, errno, strerror(errno)));
        }

        close(shm_fd); // file-descriptor is no longer needed. The mapping is retained.
        return first_copy;
#else
        static_assert(false, "OS does not provide POSIX interface for mmap(...) and munmao(...)");
#endif
    }

    void  do_deallocate(void* p, std::size_t size, size_t alignment) override {
#ifdef HAS_POSIX_MAP_INTERFACE
        if (munmap(p, size) == -1) {
            throw std::runtime_error(fmt::format("double_mapped_memory_resource::do_deallocate(void*, {}, {}) - munmap(..) failed", size, alignment));
        }
#endif
    }

    bool  do_is_equal(const memory_resource& other) const noexcept override { return this == &other; }

public:
    static inline double_mapped_memory_resource* defaultAllocator()
    {
        static double_mapped_memory_resource instance = double_mapped_memory_resource();
        return &instance;
    }

    template<typename T>
    requires util::is_power2_v<sizeof(T)>
    static inline std::pmr::polymorphic_allocator<T> allocator()
    {
        return std::pmr::polymorphic_allocator<T>(gr::double_mapped_memory_resource::defaultAllocator());
    }
};



/**
 * @brief circular buffer implementation using double-mapped memory allocations
 * where the first SIZE-ed buffer is mirrored directly its end to mimic wrap-around
 * free bulk memory access. The buffer keeps a list of indices (using 'Sequence')
 * to keep track of which parts can be tread-safely read and/or written
 *
 *                          wrap-around point
 *                                 |
 *                                 v
 *  | buffer segment #1 (original) | buffer segment #2 (copy of #1) |
 *  0                            SIZE                            2*SIZE
 *                      writeIndex
 *                          v
 *  wrap-free write access  |<-  N_1 < SIZE   ->|
 *
 *  readIndex < writeIndex-N_2
 *      v
 *      |<- N_2 < SIZE ->|
 *
 * N.B N_AVAILABLE := (SIZE + writeIndex - readIndex ) % SIZE
 *
 * citation: <find appropriate first and educational reference>
 *
 * This implementation provides single- as well as multi-producer/consumer buffer
 * combinations for thread-safe CPU-to-CPU data transfer (optionally) using either
 * a) the POSIX mmaped(..)/munmapped(..) MMU interface, if available, and/or
 * b) the guaranteed portable standard C/C++ (de-)allocators as a fall-back.
 *
 * for more details see
 */
template <typename T, std::size_t SIZE = std::dynamic_extent, ProducerType producer_type = ProducerType::Single, WaitStrategy WAIT_STRATEGY = SleepingWaitStrategy>
requires util::is_power2_v<sizeof(T)>
class buffer_host
{
    using Allocator         = std::pmr::polymorphic_allocator<T>;
    using BufferType        = buffer_host<T, SIZE, producer_type, WAIT_STRATEGY>;
    using ClaimType         = detail::producer_type_v<SIZE, producer_type, WAIT_STRATEGY>;
    using DependendsType    = std::shared_ptr<std::vector<std::shared_ptr<Sequence>>>;

    struct buffer_impl {
        alignas(kCacheLine) Allocator                   _allocator{};
        alignas(kCacheLine) const bool                  _is_mmap_allocated;
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<T, Allocator>   _data;
        alignas(kCacheLine) Sequence                    _cursor;
        alignas(kCacheLine) WAIT_STRATEGY               _wait_strategy = WAIT_STRATEGY();
        alignas(kCacheLine) ClaimType                   _claim_strategy;
        // list of dependent reader indices
        alignas(kCacheLine) DependendsType              _read_indices{ std::make_shared<std::vector<std::shared_ptr<Sequence>>>() };

        buffer_impl() = delete;
        buffer_impl(const std::size_t min_size, Allocator allocator) : _allocator(allocator), _is_mmap_allocated(dynamic_cast<double_mapped_memory_resource *>(_allocator.resource())),
            _size(align_with_page_size(min_size, _is_mmap_allocated)), _data(buffer_size(_size, _is_mmap_allocated), _allocator), _claim_strategy(ClaimType(_cursor, _wait_strategy, _size)) {
        }

        static std::size_t align_with_page_size(const std::size_t min_size, bool _is_mmap_allocated) {
            return _is_mmap_allocated ? util::round_up(min_size * sizeof(T), getpagesize()) / sizeof(T) : min_size;
        }

        static std::size_t buffer_size(const std::size_t size, bool _is_mmap_allocated) {
            // double-mmaped behaviour requires the different size/alloc strategy
            // i.e. the second buffer half may not default-constructed as it's identical to the first one
            // and would result in a double dealloc during the default destruction
            return _is_mmap_allocated ? size : 2 * size;
        }
    };

    template <typename U = T>
    class buffer_writer {
        using BufferType = std::shared_ptr<buffer_impl>;

        alignas(kCacheLine) BufferType                  _buffer; // controls buffer life-cycle, the rest are cache optimisations
        alignas(kCacheLine) const bool                  _is_mmap_allocated;
        alignas(kCacheLine) DependendsType&             _read_indices;
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<U, Allocator>&  _data;
        alignas(kCacheLine) ClaimType&                  _claim_strategy;

    public:
        buffer_writer() = delete;
        buffer_writer(std::shared_ptr<buffer_impl> buffer) :
            _buffer(buffer), _is_mmap_allocated(_buffer->_is_mmap_allocated), _read_indices(buffer->_read_indices),
            _size(buffer->_size), _data(buffer->_data), _claim_strategy(buffer->_claim_strategy) { };

        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr void publish(Translator&& translator, std::size_t n_slots_to_claim = 1, Args&&... args) {
            if (n_slots_to_claim <= 0 || _read_indices->empty()) {
                return;
            }
            const auto sequence = _claim_strategy.next(*_read_indices, n_slots_to_claim);
            translate_and_publish(std::forward<Translator>(translator), n_slots_to_claim, sequence, std::forward<Args>(args)...);
        }; // blocks until elements are available

        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr bool try_publish(Translator&& translator, std::size_t n_slots_to_claim = 1, Args&&... args) {
            if (n_slots_to_claim <= 0 || _read_indices->empty()) {
                return true;
            }
            try {
                const auto sequence = _claim_strategy.tryNext(*_read_indices, n_slots_to_claim);
                translate_and_publish(std::forward<Translator>(translator), n_slots_to_claim, sequence, std::forward<Args>(args)...);
                return true;
            } catch (const NoCapacityException &) {
                return false;
            }
        };

        [[nodiscard]] constexpr std::size_t available() const noexcept {
            return _claim_strategy.getRemainingCapacity(*_read_indices);
        }

        private:
        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr void translate_and_publish(Translator&& translator, const std::size_t n_slots_to_claim, const std::int64_t publishSequence, const Args&... args) {
            try {
                const auto index = (publishSequence + _size - n_slots_to_claim) % _size;
                std::span<U> writable_data = { &_data[index], n_slots_to_claim };
                if constexpr (std::is_invocable<Translator, std::span<T>&, std::int64_t, Args...>::value) {
                    std::invoke(std::forward<Translator>(translator), std::forward<std::span<T>&>(writable_data), publishSequence - n_slots_to_claim, args...);
                } else {
                    std::invoke(std::forward<Translator>(translator), std::forward<std::span<T>&>(writable_data), args...);
                }

                if (!_is_mmap_allocated) {
                    // mirror samples below/above the buffer's wrap-around point
                    const size_t nFirstHalf = std::min(_size - index, n_slots_to_claim);
                    const size_t nSecondHalf = n_slots_to_claim  - nFirstHalf;

                    std::copy(&_data[index], &_data[index + nFirstHalf], &_data[index+ _size]);
                    std::copy(&_data[_size],  &_data[_size + nSecondHalf], &_data[0]);
                }
                _claim_strategy.publish(publishSequence); // points at first non-writable index
            } catch (const std::exception& e) {
                throw (e);
            } catch (...) {
                throw std::runtime_error("buffer_host::translate_and_publish() - unknown user exception thrown");
            }
        }
    };

    template<typename U = T>
    class buffer_reader
    {
        using BufferType = std::shared_ptr<buffer_impl>;

        alignas(kCacheLine) std::shared_ptr<Sequence>   _read_index = std::make_shared<Sequence>();
        alignas(kCacheLine) Sequence&                   _read_index_ref;
        alignas(kCacheLine) std::int64_t                _read_index_cached;
        alignas(kCacheLine) BufferType                  _buffer; // controls buffer life-cycle, the rest are cache optimisations
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<U, Allocator>&  _data;
        alignas(kCacheLine) Sequence&                   _cursor_ref;

    public:
        buffer_reader() = delete;
        buffer_reader(std::shared_ptr<buffer_impl> buffer) : _read_index_ref(*_read_index),
            _buffer(buffer), _size(buffer->_size), _data(buffer->_data), _cursor_ref(buffer->_cursor) {
            gr::detail::addSequences(_buffer->_read_indices, _buffer->_cursor, {_read_index});
            _read_index_cached = _read_index->value();
        }
        ~buffer_reader() { gr::detail::removeSequence( _buffer->_read_indices, _read_index); }

        template <bool strict_check = true>
        [[nodiscard]] constexpr std::span<const U> get(const std::size_t n_requested = 0) const noexcept {
            if constexpr (strict_check) {
                const std::size_t n = n_requested > 0 ? std::min(n_requested, available()) : available();
                return { &_data[_read_index_cached % _size], n };
            }
            const std::size_t n = n_requested > 0 ? n_requested : available();
            return { &_data[_read_index_cached % _size], n };
        }

        template <bool strict_check = true>
        [[nodiscard]] constexpr bool consume(const std::size_t n_elements = 1) noexcept {
            if constexpr (strict_check) {
                if (n_elements <= 0) {
                    return true;
                }
                if (n_elements > available()) {
                    return false;
                }
            }
            _read_index_cached = _read_index_ref.addAndGet(n_elements);
            return true;
        }

        [[nodiscard]] constexpr std::int64_t position() const noexcept { return _read_index_cached; }

        [[nodiscard]] constexpr std::size_t available() const noexcept {
            return _cursor_ref.value() - _read_index_cached;
        }
    };

    [[nodiscard]] constexpr static Allocator DefaultAllocator() {
        if constexpr (has_posix_mmap_interface) {
            return double_mapped_memory_resource::allocator<T>();
        } else {
            return Allocator();
        }
    }

    std::shared_ptr<buffer_impl> _shared_buffer_ptr;

public:
    buffer_host() = delete;
    buffer_host(std::size_t min_size, Allocator allocator = DefaultAllocator())
        : _shared_buffer_ptr(std::make_shared<buffer_impl>(min_size, allocator)) { }
    ~buffer_host() = default;

    [[nodiscard]] std::size_t       size() const noexcept { return _shared_buffer_ptr->_size; }
    [[nodiscard]] BufferWriter auto new_writer() { return buffer_writer<T>(_shared_buffer_ptr); }
    [[nodiscard]] BufferReader auto new_reader() { return buffer_reader<T>(_shared_buffer_ptr); }

    // implementation specific interface -- not part of public Buffer / production-code API
    [[nodiscard]] auto n_readers()       { return _shared_buffer_ptr->_read_indices->size(); }
    [[nodiscard]] auto claim_strategy()  { return _shared_buffer_ptr->_claim_strategy; }
    [[nodiscard]] auto wait_strategy()   { return _shared_buffer_ptr->_wait_strategy; }
    [[nodiscard]] auto cursor_sequence() { return _shared_buffer_ptr->_cursor; }

};
static_assert(Buffer<buffer_host<int32_t>>);
// clang-format on

} // namespace gr

#endif // GNURADIO_BUFFER_HOST_HPP
