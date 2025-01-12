/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* Platform detection & Platform-specific includes */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define __GR_TARGET_WIN__
#include <windows.h>
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) ||     \
    defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__GNU__) || \
    defined(__NetBSD__) || defined(__OpenBSD__)
#define __GR_TARGET_BSD__
#include <pthread.h>
#ifdef __OpenBSD__
#include <pthread_np.h>
#endif
#else
#define __GR_TARGET_DEFAULT__
#include <pthread.h>
#include <sys/prctl.h>
#include <regex>
#endif

/* Platform-independent includes */

#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>

namespace gr {
namespace thread {

gr::logger& thread_logger()
{
    static gr::logger log("thread");
    return log;
}

gr_thread_t get_current_thread_id()
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_BSD__)
    return pthread_self();
#elif defined(__GR_TARGET_WIN__)
    return GetCurrentThread();
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "()");
    return -1;
#endif
}

void thread_bind_to_processor(int n)
{
    std::vector<int> mask(1, n);
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_WIN__)
    thread_bind_to_processor(get_current_thread_id(), mask);
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(int)");
#endif
}

void thread_bind_to_processor(const std::vector<int>& mask)
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_WIN__)
    thread_bind_to_processor(get_current_thread_id(), mask);
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(vector<int>)");
#endif
}

void thread_bind_to_processor(gr_thread_t thread, int n)
{
    std::vector<int> mask(1, n);
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_WIN__)
    thread_bind_to_processor(thread, mask);
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(gr_thread_t, int)");
#endif
}

void thread_bind_to_processor(gr_thread_t thread, const std::vector<int>& mask)
{
#if defined(__GR_TARGET_DEFAULT__)
    cpu_set_t set;
    size_t len = sizeof(cpu_set_t);

    CPU_ZERO(&set);
    for (const auto& entry : mask) {
        CPU_SET(entry, &set);
    }

    int ret = pthread_setaffinity_np(thread, len, &set);
    if (ret != 0) {
        auto msg = fmt::format("thread_bind_to_processor failed with error: {}", ret);
        thread_logger().error(msg);
        throw std::runtime_error(msg);
    }
#elif defined(__GR_TARGET_WIN__)
    DWORD_PTR dword_mask = 0;
    for (const auto& entry : mask) {
        dword_mask |= (1 << (entry));
    }

    DWORD_PTR ret = SetThreadAffinityMask(thread, dword_mask);
    if (ret == 0) {
        auto msg = fmt::format(
            "{}: SetThreadAffinityMask failed with error: {}", __func__, GetLastError());
        thread_logger().error(msg);
        throw std::runtime_error(msg);
    }
#else
    thread_logger().error("function {}{} not implemented on this platform",
                          __func__,
                          "(gr_thread_t, vector<int>)");
#endif
}

void thread_unbind()
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_WIN__)
    thread_unbind(get_current_thread_id());
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "()");
#endif
}

void thread_unbind(gr_thread_t thread)
{
#if defined(__GR_TARGET_DEFAULT__)
    cpu_set_t set;
    size_t len = sizeof(cpu_set_t);

    CPU_ZERO(&set);
    long ncpus = sysconf(_SC_NPROCESSORS_ONLN);
    for (long n = 0; n < ncpus; n++) {
        CPU_SET(n, &set);
    }

    int ret = pthread_setaffinity_np(thread, len, &set);
    if (ret != 0) {
        auto msg =
            fmt::format("{}: pthread_setaffinity_np failed with error {}", __func__, ret);
        thread_logger().error(msg);
        throw std::runtime_error(msg);
    }
#elif defined(__GR_TARGET_WIN__)
    DWORD_PTR dword_mask = sizeof(DWORD_PTR) - 1;
    DWORD_PTR ret = SetThreadAffinityMask(thread, dword_mask);
    if (ret == 0) {
        auto msg = fmt::format("thread_unbind failed with error: {}", GetLastError());
        thread_logger().error(msg);
        throw std::runtime_error(msg);
    }
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(gr_thread_t)");
#endif
}

int thread_priority(gr_thread_t thread)
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_BSD__)
    sched_param param;
    int priority;
    int policy;
    int ret;
    ret = pthread_getschedparam(thread, &policy, &param);
    priority = param.sched_priority;
    return (ret == 0) ? priority : ret;
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(gr_thread_t)");
    return -1;
#endif
}

int set_thread_priority(gr_thread_t thread, int priority)
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__GR_TARGET_BSD__)
    int policy;
    struct sched_param param;
    auto ret = pthread_getschedparam(thread, &policy, &param);
    if (ret) {
        thread_logger().error(
            "set_thread_priority: pthread_getschedparam failed with return code {}", ret);
        return ret;
    }
    param.sched_priority = priority;
    ret = pthread_setschedparam(thread, policy, &param);
    if (ret) {
        thread_logger().error(
            "set_thread_priority: pthread_setschedparam failed with return code {}", ret);
    }
    return ret;
#else
    thread_logger().error(
        "function {}{} not implemented on this platform", __func__, "(gr_thread_t, int)");
    return -1;
#endif
}

void set_thread_name(gr_thread_t thread, std::string name)
{
#if defined(__GR_TARGET_DEFAULT__) || defined(__APPLE__)
    if (thread != pthread_self()) {
        thread_logger().error("Trying to set thread name from different thread: Naming "
                              "another thread is not supported on this platform.");
        return;
    }
#endif

    if (name.empty())
        name = "thread " + std::to_string((unsigned long long)thread);

#if defined(__GR_TARGET_DEFAULT__)
    // Maximum accepted by PR_SET_NAME **EXCL ZERO TERMINATOR**
    constexpr unsigned int MAX_PR_SET_NAME_LENGTH = 15;
    /* Shorten the name if necessary by taking as many characters from the front so that
     * the unique_id can still fit on the end.
     */
    if (name.size() > MAX_PR_SET_NAME_LENGTH) {
        // matches "blockname1234" and "block1name" and "blockname",
        // but not "1234"
        std::string re_string{ "^(.*[^0-9])([0-9]*)$" };
        std::smatch match;
        std::regex re{ re_string, std::regex::extended };
        bool success = std::regex_match(name, match, re);
        if (success) {
            // match = { full string, block alias, number at end (or empty) }
            const auto number = match[2];
            const unsigned number_length = number.length();
            if (number_length > MAX_PR_SET_NAME_LENGTH) {
                // number alone fills space – just use beginning of number
                name = number.str().substr(0, MAX_PR_SET_NAME_LENGTH);
            } else {
                name = match[1].str().substr(0, MAX_PR_SET_NAME_LENGTH - number_length) +
                       number.str();
            }
        } else {
            name = name.substr(0, MAX_PR_SET_NAME_LENGTH);
        }
    }

    prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#elif defined(__GR_TARGET_BSD__)
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    pthread_set_name_np(thread, name.c_str());
#elif defined(__APPLE__)
    pthread_setname_np(name.c_str());
#elif defined(__NetBSD__)
    pthread_setname_np(thread, name.c_str(), nullptr);
#else
    thread_logger().error("function {}{} not implemented on this BSD-alike platform",
                          __func__,
                          "(gr_thread_t, string)");
#endif
#elif defined(__GR_TARGET_WIN__) && (!defined(__MINGW32__))
#pragma pack(push, 8)
    struct threadname_info {
        DWORD dwType;     // Must be 0x1000
        LPCSTR szName;    // Pointer to name (in user addr space)
        DWORD dwThreadID; // Thread ID (-1 = caller thread)
        DWORD dwFlags;    // Reserved for future use, must be zero
    };
#pragma pack(pop)
    DWORD dwThreadId = GetThreadId(thread);
    if (dwThreadId == 0) {
        thread_logger().error("{}: could not GetThreadId({})", __func__, thread);
        return;
    }

    threadname_info info;
    info.dwType = 0x1000;
    info.szName = name.c_str();
    info.dwThreadID = dwThreadId;
    info.dwFlags = 0;
    /* The following is what I'd consider a dirty hack – but this is targeting windows,
     * after all. We use the microsoft `__try` construct, because that's the only WIN32
     * way. However, that's not compatible with functions that actually do RAII… so,
     * incompatible with C++. (this is still a C++ API.) So, put `try__` in anonymous
     * lambda, move on, wonder why, and then decide to not cry over spilt design choices.
     */
    [&info]() {
        __try {
            constexpr DWORD SET_THREAD_NAME_EXCEPTION = 0x406D1388;
            RaiseException(SET_THREAD_NAME_EXCEPTION,
                           0,
                           sizeof(info) / sizeof(ULONG_PTR),
                           (ULONG_PTR*)&info);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }();
#else
    thread_logger().error("function {}{} not implemented on this platform",
                          __func__,
                          "(gr_thread_t, string)");
#endif
}

} /* namespace thread */
} /* namespace gr */
