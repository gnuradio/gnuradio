#ifndef GNURADIO_SEQUENCE_HPP
#define GNURADIO_SEQUENCE_HPP

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>
#include <memory>
#include <ranges>
#include <vector>

namespace gr {

#ifndef forceinline
// use this for hot-spots only <-> may bloat code size, not fit into cache and
// consequently slow down execution
#define forceinline inline __attribute__((always_inline))
#endif

static constexpr const std::size_t kCacheLine = 64;
static constexpr const std::int64_t kInitialCursorValue = -1L;

/**
 * Concurrent sequence class used for tracking the progress of the ring buffer and event
 * processors. Support a number of concurrent operations including CAS and order writes.
 * Also attempts to be more efficient with regards to false sharing by adding padding
 * around the volatile field.
 */
// clang-format off
class Sequence
{
    alignas(kCacheLine) std::atomic<std::int64_t> _fieldsValue{};

public:
    Sequence(const Sequence&) = delete;
    Sequence(const Sequence&&) = delete;
    void operator=(const Sequence&) = delete;
    explicit Sequence(std::int64_t initialValue = kInitialCursorValue) noexcept
        : _fieldsValue(initialValue)
    {
    }

    [[nodiscard]] forceinline std::int64_t value() const noexcept
    {
        return std::atomic_load_explicit(&_fieldsValue, std::memory_order_acquire);
    }

    forceinline void setValue(std::int64_t value) noexcept
    {
        std::atomic_store_explicit(&_fieldsValue, value, std::memory_order_release);
    }

    [[nodiscard]] forceinline bool compareAndSet(std::int64_t expectedSequence,
                                                 std::int64_t nextSequence) noexcept
    {
        // atomically set the value to the given updated value if the current value == the
        // expected value (true, otherwise folse).
        return std::atomic_compare_exchange_strong(
            &_fieldsValue, &expectedSequence, nextSequence);
    }

    [[nodiscard]] forceinline std::int64_t incrementAndGet() noexcept
    {
        return std::atomic_fetch_add(&_fieldsValue, 1L) + 1L;
    }

    [[nodiscard]] forceinline std::int64_t addAndGet(std::int64_t value) noexcept
    {
        return std::atomic_fetch_add(&_fieldsValue, value) + value;
    }
};

namespace detail {
/**
 * Get the minimum sequence from an array of Sequences.
 *
 * \param sequences sequences to compare.
 * \param minimum an initial default minimum.  If the array is empty this value will
 * returned. \returns the minimum sequence found or lon.MaxValue if the array is empty.
 */
inline std::int64_t getMinimumSequence(
    const std::vector<std::shared_ptr<Sequence>>& sequences,
    std::int64_t minimum = std::numeric_limits<std::int64_t>::max()) noexcept
{
    if (sequences.empty()) {
        return minimum;
    }
    return std::min(
        minimum,
        std::ranges::min(sequences, std::less{}, [](const auto& sequence) noexcept {
            return sequence->value();
        })->value());
}

inline void addSequences(std::shared_ptr<std::vector<std::shared_ptr<Sequence>>>& sequences,
             const Sequence& cursor,
             const std::vector<std::shared_ptr<Sequence>>& sequencesToAdd)
{
    std::int64_t cursorSequence;
    std::shared_ptr<std::vector<std::shared_ptr<Sequence>>> updatedSequences;
    std::shared_ptr<std::vector<std::shared_ptr<Sequence>>> currentSequences;

    do {
        currentSequences = std::atomic_load_explicit(&sequences, std::memory_order_acquire);
        updatedSequences = std::make_shared<std::vector<std::shared_ptr<Sequence>>>(currentSequences->size() + sequencesToAdd.size());

        std::ranges::copy(currentSequences->begin(), currentSequences->end(), updatedSequences->begin());

        cursorSequence = cursor.value();

        auto index = currentSequences->size();
        for (auto&& sequence : sequencesToAdd) {
            sequence->setValue(cursorSequence);
            (*updatedSequences)[index] = sequence;
            index++;
        }
    } while (!std::atomic_compare_exchange_weak(&sequences, &currentSequences, updatedSequences)); // xTODO: explicit memory order

    cursorSequence = cursor.value();

    for (auto&& sequence : sequencesToAdd) {
        sequence->setValue(cursorSequence);
    }
}

inline bool removeSequence(std::shared_ptr<std::vector<std::shared_ptr<Sequence>>>& sequences, const std::shared_ptr<Sequence>& sequence)
{
    std::uint32_t numToRemove;
    std::shared_ptr<std::vector<std::shared_ptr<Sequence>>> oldSequences;
    std::shared_ptr<std::vector<std::shared_ptr<Sequence>>> newSequences;

    do {
        oldSequences = std::atomic_load_explicit(&sequences, std::memory_order_acquire);
        numToRemove = static_cast<std::uint32_t>(
            std::ranges::count_if(*oldSequences, [&sequence](const auto& value) {
                return value == sequence;
            })); // specifically uses identity
        if (numToRemove == 0) {
            break;
        }

        auto oldSize = static_cast<std::uint32_t>(oldSequences->size());
        newSequences = std::make_shared<std::vector<std::shared_ptr<Sequence>>>(
            oldSize - numToRemove);

        for (auto i = 0U, pos = 0U; i < oldSize; ++i) {
            const auto& testSequence = (*oldSequences)[i];
            if (sequence != testSequence) {
                (*newSequences)[pos] = testSequence;
                pos++;
            }
        }
    } while (!std::atomic_compare_exchange_weak(&sequences, &oldSequences, newSequences));

    return numToRemove != 0;
}
// clang-format om

} // namespace detail

} // namespace gr

#ifdef FMT_FORMAT_H_

template <>
struct fmt::formatter<gr::Sequence> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(gr::Sequence const& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{}", value.value());
    }
};

namespace gr {
inline std::ostream& operator<<(std::ostream& os, const Sequence& v)
{
    return os << fmt::format("{}", v);
}
} // namespace gr


#endif // FMT_FORMAT_H_

#endif // GNURADIO_SEQUENCE_HPP
