// Boost portions copied from runtime_symbol_info.hpp

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015-2019 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <filesystem>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <boost/dll/detail/windows/path_from_handle.hpp>
#include <boost/winapi/dll.hpp>
#else
#include <dlfcn.h>
#endif


namespace fs = std::filesystem;
namespace gr {
/*!
 * On success returns full path and name to the binary object that holds symbol pointed by
 * ptr_to_symbol.
 *
 * \param ptr_to_symbol Pointer to symbol which location is to be determined.
 * \return Path to the binary object that holds symbol or empty path in case error.
 * \throws std::bad_alloc in case of insufficient memory. Overload that does not accept
 * \forcedlinkfs{error_code} also throws \forcedlinkfs{system_error}.
 *
 * \b Examples:
 * \code
 * int main() {
 *    dll::symbol_location_ptr(std::set_terminate(0));       // returns
 * "/some/path/libmy_terminate_handler.so" dll::symbol_location_ptr(::signal(SIGSEGV,
 * SIG_DFL));  // returns "/some/path/libmy_symbol_handler.so"
 * }
 * \endcode
 */
template <class T>
inline fs::path symbol_location_ptr(T ptr_to_symbol)
{
    fs::path ret;
    if (!ptr_to_symbol) {
        throw std::runtime_error("symbol_location_ptr given bad function ptr");
    }

    const void* ptr = reinterpret_cast<const void*>(ptr_to_symbol);

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    // FIXME: Replace Boost Here ...
    boost::winapi::MEMORY_BASIC_INFORMATION_ mbi;
    if (!boost::winapi::VirtualQuery(ptr, &mbi, sizeof(mbi))) {
        ec = boost::dll::detail::last_error_code();
        return ret;
    }

    return boost::dll::detail::path_from_handle(
        reinterpret_cast<boost::winapi::HMODULE_>(mbi.AllocationBase), ec);
#else
    Dl_info info;

    // Some of the libc headers miss `const` in `dladdr(const void*, Dl_info*)`
    const int res = dladdr(const_cast<void*>(ptr), &info);

    if (res) {
        ret = info.dli_fname;
    }
    else {
        throw std::runtime_error("dladdr returned bad address");
    }

    return ret;
#endif
}

} // namespace gr