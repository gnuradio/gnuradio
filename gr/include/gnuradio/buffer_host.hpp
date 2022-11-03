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
constexpr int round_up(int numToRound, int multiple) noexcept
{
    if (multiple == 0) {
        return numToRound;
    }
    const int remainder = numToRound % multiple;
    if (remainder == 0) {
        return numToRound;
    }
    return numToRound + multiple - remainder;
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
template <typename T, std::size_t SIZE = std::dynamic_extent, ProducerType producerType = ProducerType::Single, WaitStrategy WAIT_STRATEGY = SleepingWaitStrategy>
requires util::is_power2_v<sizeof(T)>
class buffer_host
{
    using Allocator         = std::pmr::polymorphic_allocator<T>;
    using BufferType        = buffer_host<T, SIZE, producerType, WAIT_STRATEGY>;
    using ClaimType         = detail::producer_type_v<SIZE, producerType, WAIT_STRATEGY>;
    using DependendsType    = std::shared_ptr<std::vector<std::shared_ptr<Sequence>>>;

    struct buffer_impl {
        alignas(kCacheLine) Allocator                   _allocator{};
        alignas(kCacheLine) const bool                  _is_mmap_allocated;
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<T, Allocator>   _data;
        alignas(kCacheLine) Sequence                    _cursor;
        alignas(kCacheLine) WAIT_STRATEGY               _waitStrategy = WAIT_STRATEGY();
        alignas(kCacheLine) ClaimType                   _claimStrategy;
        // list of dependent reader indices
        alignas(kCacheLine) DependendsType              _readIndices{ std::make_shared<std::vector<std::shared_ptr<Sequence>>>() };

        buffer_impl() = delete;
        buffer_impl(const std::size_t min_size, Allocator allocator) : _allocator(allocator), _is_mmap_allocated(dynamic_cast<double_mapped_memory_resource *>(_allocator.resource())),
            _size(alignWithPageSize(min_size, _is_mmap_allocated)), _data(bufferSize(_size, _is_mmap_allocated), _allocator), _claimStrategy(ClaimType(_cursor, _waitStrategy, _size)) {
        }

        static std::size_t alignWithPageSize(const std::size_t min_size, bool _is_mmap_allocated) {
            return _is_mmap_allocated ? util::round_up(min_size * sizeof(T), getpagesize()) / sizeof(T) : min_size;
        }

        static std::size_t bufferSize(const std::size_t size, bool _is_mmap_allocated) {
            // double-mmaped behaviour requires the different size/alloc strategy
            // i.e. the second buffer half may not default-constructed as it's identical to the first one
            // and would result in a double dealloc during the default destruction
            return _is_mmap_allocated ? size : 2 * size;
        }
    };

    template <typename U = T>
    class buffer_host_writer {
        using BufferType = std::shared_ptr<buffer_impl>;

        alignas(kCacheLine) BufferType                  _buffer; // controls buffer life-cycle, the rest are cache optimisations
        alignas(kCacheLine) const bool                  _is_mmap_allocated;
        alignas(kCacheLine) DependendsType&             _readIndices;
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<U, Allocator>&  _data;
        alignas(kCacheLine) ClaimType&                  _claimStrategy;

    public:
        buffer_host_writer() = delete;
        buffer_host_writer(std::shared_ptr<buffer_impl> buffer) :
            _buffer(buffer), _is_mmap_allocated(_buffer->_is_mmap_allocated), _readIndices(buffer->_readIndices),
            _size(buffer->_size), _data(buffer->_data), _claimStrategy(buffer->_claimStrategy) { };

        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr void publish(Translator&& translator, std::size_t n_slots_to_claim = 1, Args&&... args) {
            if (n_slots_to_claim <= 0 || _readIndices->empty()) {
                return;
            }
            const auto sequence = _claimStrategy.next(*_readIndices, n_slots_to_claim);
            translateAndPublish(std::forward<Translator>(translator), n_slots_to_claim, sequence, std::forward<Args>(args)...);
        }; // blocks until elements are available

        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr bool tryPublish(Translator&& translator, std::size_t n_slots_to_claim = 1, Args&&... args) {
            if (n_slots_to_claim <= 0 || _readIndices->empty()) {
                return true;
            }
            try {
                const auto sequence = _claimStrategy.tryNext(*_readIndices, n_slots_to_claim);
                translateAndPublish(std::forward<Translator>(translator), n_slots_to_claim, sequence, std::forward<Args>(args)...);
                return true;
            } catch (const NoCapacityException &) {
                return false;
            }
        };

        [[nodiscard]] constexpr std::size_t available() const noexcept {
            return _claimStrategy.getRemainingCapacity(*_readIndices);
        }

        private:
        template <typename... Args, WriterCallback<U, Args...> Translator>
        constexpr void translateAndPublish(Translator&& translator, const std::size_t n_slots_to_claim, const std::int64_t publishSequence, const Args&... args) {
            try {
                const auto index = (publishSequence + _size - n_slots_to_claim) % _size;
                std::span<U> writableData = { &_data[index], n_slots_to_claim };
                if constexpr (std::is_invocable<Translator, std::span<T>&, std::int64_t, Args...>::value) {
                    std::invoke(std::forward<Translator>(translator), std::forward<std::span<T>&>(writableData), publishSequence - n_slots_to_claim, args...);
                } else {
                    std::invoke(std::forward<Translator>(translator), std::forward<std::span<T>&>(writableData), args...);
                }

                if (!_is_mmap_allocated) {
                    // mirror samples below/above the buffer's wrap-around point
                    const size_t nFirstHalf = std::min(_size - index, n_slots_to_claim);
                    const size_t nSecondHalf = n_slots_to_claim  - nFirstHalf;

                    std::copy(&_data[index], &_data[index + nFirstHalf], &_data[index+ _size]);
                    std::copy(&_data[_size],  &_data[_size + nSecondHalf], &_data[0]);
                }
                _claimStrategy.publish(publishSequence); // points at first non-writable index
            } catch (const std::exception& e) {
                throw (e);
            } catch (...) {
                throw std::runtime_error("buffer_host::translateAndPublish() - unknown user exception thrown");
            }
        }
    };

    template<typename U = T>
    class buffer_host_reader
    {
        using BufferType = std::shared_ptr<buffer_impl>;

        alignas(kCacheLine) std::shared_ptr<Sequence>   _readIndex = std::make_shared<Sequence>();
        alignas(kCacheLine) Sequence&                   _readIndexRef;
        alignas(kCacheLine) std::int64_t                _readIndexCached;
        alignas(kCacheLine) BufferType                  _buffer; // controls buffer life-cycle, the rest are cache optimisations
        alignas(kCacheLine) const std::size_t           _size;
        alignas(kCacheLine) std::vector<U, Allocator>&  _data;
        alignas(kCacheLine) Sequence&                   _cursorRef;

    public:
        buffer_host_reader() = delete;
        buffer_host_reader(std::shared_ptr<buffer_impl> buffer) : _readIndexRef(*_readIndex),
            _buffer(buffer), _size(buffer->_size), _data(buffer->_data), _cursorRef(buffer->_cursor) {
            gr::detail::addSequences(_buffer->_readIndices, _buffer->_cursor, {_readIndex});
            _readIndexCached = _readIndex->value();
        }
        ~buffer_host_reader() { gr::detail::removeSequence( _buffer->_readIndices, _readIndex); }

        template <bool strict_check = true>
        [[nodiscard]] constexpr std::span<const U> get(const std::size_t n_requested = 0) const noexcept {
            if constexpr (strict_check) {
                const std::size_t n = n_requested > 0 ? std::min(n_requested, available()) : available();
                return { &_data[_readIndexCached % _size], n };
            }
            const std::size_t n = n_requested > 0 ? n_requested : available();
            return { &_data[_readIndexCached % _size], n };
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
            _readIndexCached = _readIndexRef.addAndGet(n_elements);
            return true;
        }

        [[nodiscard]] constexpr std::int64_t position() const noexcept { return _readIndexCached; }

        [[nodiscard]] constexpr std::size_t available() const noexcept {
            return _cursorRef.value() - _readIndexCached;
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
    [[nodiscard]] BufferWriter auto newWriterInstance() { return buffer_host_writer<T>(_shared_buffer_ptr); }
    [[nodiscard]] BufferReader auto newReaderInstance() { return buffer_host_reader<T>(_shared_buffer_ptr); }

    // implementation specific interface -- not part of public Buffer / production-code API
    [[nodiscard]] auto nReaders()       { return _shared_buffer_ptr->_readIndices->size(); }
    [[nodiscard]] auto claimStrategy()  { return _shared_buffer_ptr->_claimStrategy; }
    [[nodiscard]] auto waitStrategy()   { return _shared_buffer_ptr->_waitStrategy; }
    [[nodiscard]] auto cursorSequence() { return _shared_buffer_ptr->_cursor; }

};
static_assert(Buffer<buffer_host<int32_t>>);
// clang-format on

} // namespace gr

#endif // GNURADIO_BUFFER_HOST_HPP
