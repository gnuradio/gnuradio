#ifndef GNURADIO_BUFFER2_H
#define GNURADIO_BUFFER2_H

#include <type_traits>
#include <concepts>
#include <span>

namespace gr {
namespace util { // TODO: move this meta-programming helpers to a common header file
template <typename T, typename...>
struct first_template_arg_helper;

template <template <typename...> class TemplateType,
          typename ValueType,
          typename... OtherTypes>
struct first_template_arg_helper<TemplateType<ValueType, OtherTypes...>> {
    using value_type = ValueType;
};

template <typename T>
constexpr auto* value_type_helper()
{
    if constexpr (requires { typename T::value_type; }) {
        return static_cast<typename T::value_type*>(nullptr);
    }
    else {
        return static_cast<typename first_template_arg_helper<T>::value_type*>(nullptr);
    }
}

template <typename T>
using value_type_t = std::remove_pointer_t<decltype(value_type_helper<T>())>;

template <typename... A>
struct test_fallback {
};

template <typename, typename ValueType>
struct test_value_type {
    using value_type = ValueType;
};

static_assert(std::is_same_v<int, value_type_t<test_fallback<int, float, double>>>);
static_assert(std::is_same_v<float, value_type_t<test_value_type<int, float>>>);
static_assert(std::is_same_v<int, value_type_t<std::span<int>>>);
static_assert(std::is_same_v<double, value_type_t<std::array<double, 42>>>);

constexpr bool isPower2(std::unsigned_integral auto value)
{
    return !(value == 0) && !(value & (value - 1));
}
template <std::unsigned_integral auto N>
static constexpr bool is_power2_v = isPower2(N);

static_assert(is_power2_v<1U>);
static_assert(is_power2_v<2U>);
static_assert(!is_power2_v<0U>);
static_assert(!is_power2_v<3U>);
} // namespace util

// clang-format off
// disable formatting until clang-format (v16) supporting concepts
template<class T>
concept BufferReader = requires(T /*const*/ t, const std::size_t n_items) {
    { t.get(n_items) }     -> std::same_as<std::span<const util::value_type_t<T>>>;
    { t.consume(n_items) } -> std::same_as<bool>;
    { t.position() }       -> std::same_as<std::int64_t>;
    { t.available() }      -> std::same_as<std::size_t>;
};

template<class Fn, typename T, typename ...Args>
concept WriterCallback = std::is_invocable<Fn, std::span<T>&, std::int64_t, Args...>::value || std::is_invocable<Fn, std::span<T>&, Args...>::value;

template<class T, typename ...Args>
concept BufferWriter = requires(T t, const std::size_t n_items, Args ...args) {
    { t.publish([](std::span<util::value_type_t<T>> &writableData, Args ...) {}, n_items, args...) }                                -> std::same_as<void>;
    { t.publish([](std::span<util::value_type_t<T>> &writableData, std::int64_t /* writePos */, Args ...) {}, n_items, args...) }   -> std::same_as<void>;
    { t.tryPublish([](std::span<util::value_type_t<T>> &writableData, Args ...) {}, n_items, args...) }                             -> std::same_as<bool>;
    { t.tryPublish([](std::span<util::value_type_t<T>> &writableData, std::int64_t /* writePos */, Args ...) {}, n_items, args...) }-> std::same_as<bool>;
    { t.available() }         -> std::same_as<std::size_t>;
};

template<class T, typename U1 = T, typename U2 = T>
concept Buffer = requires(T t, const std::size_t min_size) {
    { T(min_size) };
    { t.size() }              -> std::same_as<std::size_t>;
    { t.newReaderInstance() } -> BufferReader;
    { t.newWriterInstance() } -> BufferWriter;
};

// compile-time unit-tests
namespace test {
template <typename T>
struct non_compliant_class {
};
template <typename T, typename... Args>
using WithSequenceParameter = decltype([](std::span<T>&, std::int64_t, Args...) {});
template <typename T, typename... Args>
using NoSequenceParameter = decltype([](std::span<T>&, Args...) {});
} // namespace test

static_assert(!Buffer<test::non_compliant_class<int>>);
static_assert(!BufferReader<test::non_compliant_class<int>>);
static_assert(!BufferWriter<test::non_compliant_class<int>>);

static_assert(WriterCallback<test::WithSequenceParameter<int>, int>);
static_assert(!WriterCallback<test::WithSequenceParameter<int>, int, std::span<bool>>);
static_assert(WriterCallback<test::WithSequenceParameter<int, std::span<bool>>, int, std::span<bool>>);
static_assert(WriterCallback<test::NoSequenceParameter<int>, int>);
static_assert(!WriterCallback<test::NoSequenceParameter<int>, int, std::span<bool>>);
static_assert(WriterCallback<test::NoSequenceParameter<int, std::span<bool>>, int, std::span<bool>>);
// clang-format on
} // namespace gr

#endif // GNURADIO_BUFFER2_H
