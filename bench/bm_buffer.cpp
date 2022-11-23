#include <barrier>
#include <chrono>
#include <iostream>
#include <thread>

#include <fmt/format.h>

#include <gnuradio/buffer.hpp> // new buffer header interface
#include <gnuradio/buffer_skeleton.hpp>
#include <gnuradio/circular_buffer.hpp>

#include <gnuradio/buffer.h>
#include <gnuradio/buffer_cpu_simple.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
#include <gnuradio/realtime.h>

#include <iostream>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

using namespace gr;

#if defined __has_include
#if __has_include(<pthread.h>) && __has_include(<sched.h>)
#include <errno.h>
#include <pthread.h>
#include <sched.h>
void setCpuAffinity(const int cpuID) // N.B. pthread is not portable
{
    const auto nCPU = std::thread::hardware_concurrency();
    // fmt::print("set CPU affinity to core {}\n", cpuID % nCPU);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpuID % nCPU, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        constexpr auto fmt = "pthread_setaffinity_np({} of {}): {} - {}\n";
        fmt::print(stderr, fmt, cpuID, nCPU, rc, strerror(rc));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // to force re-scheduling
}
#elif
void setCpuAffinity(const int cpuID) {}
#endif
#elif
void setCpuAffinity(const int cpuID) {}
#endif

auto testNewAPI(Buffer auto& buffer,
                const std::size_t vector_length,
                const std::size_t min_samples,
                const std::uint64_t nProducer,
                const std::uint64_t nConsumer,
                bool rt_prio,
                bool printCheck = true)
{
    assert(nProducer > 0);
    assert(nConsumer > 0);

    // start/stop barriers
    auto time_start = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> time_elapsed;
    auto on_completion1 = [&time_start]() noexcept {
        time_start = std::chrono::system_clock::now();
    };
    auto on_completion2 = [&time_start, &time_elapsed]() noexcept {
        time_elapsed = std::chrono::system_clock::now() - time_start;
    };
    std::barrier startMark(static_cast<long>(nProducer + nConsumer), on_completion1);
    std::barrier stopMark(static_cast<long>(nProducer + nConsumer), on_completion2);

    // init producers
    std::atomic<int> threadID = 0;
    std::vector<std::jthread> producers;
    for (auto i = 0U; i < nProducer; i++) {
        producers.emplace_back([&]() {
            BufferWriter auto writer = buffer.new_writer();
            // set thread affinity
            setCpuAffinity(threadID++);
            startMark.arrive_and_wait();
            std::size_t nSamplesProduced = 0;
            while (nSamplesProduced <= min_samples) {
                writer.publish([](auto&) {}, vector_length);
                nSamplesProduced += vector_length;
            }
            stopMark.arrive_and_wait();
        });
    }

    // init consumers
    std::vector<std::jthread> consumers;
    for (auto i = 0U; i < nConsumer; i++) {
        consumers.emplace_back([&]() {
            BufferReader auto reader = buffer.new_reader();
            setCpuAffinity(threadID++);
            startMark.arrive_and_wait();
            std::size_t nSamplesConsumed = 0;
            while (nSamplesConsumed < min_samples) {
                if (reader.available() < vector_length) {
                    continue;
                }
                const auto& input = reader.get(vector_length);
                nSamplesConsumed += input.size();

                if (!reader.consume(input.size())) {
                    throw std::runtime_error(
                        fmt::format("could not consume {} samples", input.size()));
                }
            }
            stopMark.arrive_and_wait();
            if (nSamplesConsumed < min_samples && printCheck) {
                fmt::print(
                    "finished RingBuffer consumer {} - nConsumed {} vs. min_samples\n",
                    i,
                    nSamplesConsumed,
                    min_samples);
            }
        });
    }
    std::ranges::for_each(producers, [](auto& thread) { thread.join(); });
    std::ranges::for_each(consumers, [](auto& thread) { thread.join(); });

    return time_elapsed;
}

auto testOldAPI(gr::buffer_uptr& buffer,
                auto& bufferProperties,
                const std::size_t vector_length,
                const std::size_t min_samples,
                const std::uint64_t nProducer,
                const std::uint64_t nConsumer,
                bool rt_prio,
                bool printCheck = true)
{
    assert(nProducer > 0);
    assert(nConsumer > 0);

    // start/stop barriers
    auto time_start = std::chrono::system_clock::now();
    std::chrono::duration<double, std::nano> time_elapsed;
    auto on_completion1 = [&time_start]() noexcept {
        time_start = std::chrono::system_clock::now();
    };
    auto on_completion2 = [&time_start, &time_elapsed]() noexcept {
        time_elapsed = std::chrono::system_clock::now() - time_start;
    };
    std::barrier startMark(static_cast<long>(nProducer + nConsumer), on_completion1);
    std::barrier stopMark(static_cast<long>(nProducer + nConsumer), on_completion2);

    // init producers
    int threadID = 0;
    std::vector<std::jthread> producers;
    for (auto i = 0U; i < nProducer; i++) {
        producers.emplace_back([&]() {
            // set thread affinity
            setCpuAffinity(threadID++);
            startMark.arrive_and_wait();
            std::size_t nSamplesProduced = 0;
            buffer_info_t info;
            while (nSamplesProduced <= min_samples) {
                buffer->write_info(info);
                if (static_cast<std::size_t>(info.n_items) < vector_length) {
                    continue; // insufficient items
                }
                buffer->post_write(vector_length);
                nSamplesProduced += vector_length;
            }
            stopMark.arrive_and_wait();
        });
    }

    // init consumers
    std::vector<std::jthread> consumers;
    for (auto i = 0U; i < nConsumer; i++) {
        consumers.emplace_back([&]() {
            auto reader = buffer->add_reader(bufferProperties, sizeof(int32_t));
            setCpuAffinity(threadID++);
            startMark.arrive_and_wait();
            std::size_t nSamplesConsumed = 0;
            buffer_info_t info;
            while (nSamplesConsumed < min_samples) {
                reader->read_info(info);
                if (static_cast<std::size_t>(info.n_items) < vector_length) {
                    continue; // insufficient items
                }

                nSamplesConsumed += vector_length;

                reader->post_read(vector_length);
            }
            stopMark.arrive_and_wait();
            if (nSamplesConsumed < min_samples && printCheck) {
                fmt::print(
                    "finished RingBuffer consumer {} - nConsumed {} vs. min_samples\n",
                    i,
                    nSamplesConsumed,
                    min_samples);
            }
        });
    }
    std::ranges::for_each(producers, [](auto& thread) { thread.join(); });
    std::ranges::for_each(consumers, [](auto& thread) { thread.join(); });

    return time_elapsed;
}

int main(int argc, char* argv[])
{
    uint64_t samples = 10'000'000'000;
    int nWriter = 1;
    int nReader = 4;
    int veclen = 1024;
    int buffer_type = 4;
    bool throughput = true;
    bool rt_prio = false;
    bool scan = false;

    CLI::App app{ "App description" };

    app.add_option("--samples", samples, "minimum number of samples");
    app.add_option("--veclen", veclen, "vector length (<-> size of bulk operation)");
    app.add_option("--nreader", nReader, "number of consumers");
    app.add_option("--buffer_type",
                   buffer_type,
                   "Buffer Type (0:simple, 1:vmcirc, 2:cuda, 3:cuda_pinned, 4: "
                   "circular_buffer - POSIX, 5: portable");
    app.add_flag(
        "--throughput", throughput, "whether to print samples/s rather than time");
    app.add_flag("--rt_prio", rt_prio, "enable real-time priority");
    app.add_flag("--scan", scan, "scan producer-consumer parameter space");
    CLI11_PARSE(app, argc, argv);

    if (rt_prio && gr::enable_realtime_scheduling() != rt_status_t::OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    const std::size_t sizeBuffer = std::max(4096, veclen) * nReader * 10;

    auto oneTest = [&](const int nP, const int nR) {
        using BufferType =
            circular_buffer<int32_t, std::dynamic_extent, ProducerType::Single>;

        std::chrono::duration<double, std::nano> testDuration;
        switch (buffer_type) {
        case 0: {
            if (nP > 1) {
                fmt::print("nProducer {} unsupported for b-Type {}\n", nP, buffer_type);
                return;
            }
            auto bufferProperties = buffer_cpu_simple_properties::make();
            auto buffer =
                buffer_cpu_simple::make(sizeBuffer, sizeof(int32_t), bufferProperties);
            testDuration =
                testOldAPI(buffer, bufferProperties, veclen, samples, nP, nR, rt_prio);
        } break;
        case 1: {
            if (nP > 1) {
                fmt::print("nProducer {} unsupported for b-Type {}\n", nP, buffer_type);
                return;
            }
            auto bufferProperties =
                buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO);
            auto buffer =
                buffer_cpu_vmcirc::make(sizeBuffer, sizeof(int32_t), bufferProperties);
            testDuration =
                testOldAPI(buffer, bufferProperties, veclen, samples, nP, nR, rt_prio);
        } break;
        case 4: {
            Buffer auto buffer = BufferType(sizeBuffer);
            testDuration = testNewAPI(buffer, veclen, samples, nP, nR, rt_prio);
        } break;
        case 5: {
            auto allocator = std::pmr::polymorphic_allocator<int32_t>();
            Buffer auto buffer = BufferType(sizeBuffer, allocator);
            testDuration = testNewAPI(buffer, veclen, samples, nP, nR, rt_prio);
        } break;
        default: {
            fmt::print("unknown buffer type ID: {}\n", buffer_type);
            return;
        }
        }

        auto time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(testDuration).count() /
            1e9;

        if (throughput) {
            const auto nSamples = samples * nP * nR / veclen;
            const auto opsPerSecond = static_cast<double>(nSamples) / time;
            fmt::print(
                "buffer type: {} - {}P-{}C: {} samples in {:6.2f} ms -> {:.1E} ops/s\n",
                buffer_type,
                nP,
                nR,
                nSamples,
                time * 1e3,
                opsPerSecond);
        }
        else {
            std::cout << "[PROFILE_TIME]" << time << "[PROFILE_TIME]" << std::endl;
        }
    };

    if (scan) {
        for (int p = 1; p <= nWriter; p++) {
            for (int c = 1; c <= nReader; c++) {
                oneTest(p, c);
            }
        }
    }
    else {
        oneTest(nWriter, nReader);
    }
}
