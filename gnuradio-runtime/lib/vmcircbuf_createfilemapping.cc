/* -*- c++ -*- */
/*
 * Copyright 2003,2005,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include "pagesize.h"
#include "vmcircbuf_createfilemapping.h"

namespace gr {

#ifdef HAVE_CREATEFILEMAPPING
// Print Windows error (could/should be global?)
static void werror(const char* where, DWORD last_error)
{
    char buf[1024];

    logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "werror");

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  last_error,
                  0, // default language
                  buf,
                  sizeof(buf) / sizeof(TCHAR), // buffer size
                  NULL);
    logger->error("{:s}: Error {:d}: {:s}", where, last_error, buf);
    return;
}
#endif


vmcircbuf_createfilemapping::vmcircbuf_createfilemapping(size_t size)
    : gr::vmcircbuf(size)
{
    gr::configure_default_loggers(
        d_logger, d_debug_logger, "vmcircbuf_createfilemapping");
#if !defined(HAVE_CREATEFILEMAPPING)
    d_debug_logger->info("{:s}: CreateFileMapping is not available", __FUNCTION__);
    throw std::runtime_error("gr::vmcircbuf_createfilemapping");
#else
    gr::thread::scoped_lock guard(s_vm_mutex);

    static int s_seg_counter = 0;

    if (size <= 0 || (size % gr::pagesize()) != 0) {
        d_logger->error("invalid size = {:d}", size);
        throw std::runtime_error("gr::vmcircbuf_createfilemapping");
    }

    std::string seg_name =
        "/gnuradio-" + std::to_string(getpid()) + "-" + std::to_string(s_seg_counter);

    d_handle = CreateFileMapping(INVALID_HANDLE_VALUE, // use paging file
                                 NULL,                 // default security
                                 PAGE_READWRITE,       // read/write access
                                 0,                    // max. object size
                                 size,                 // buffer size
                                 seg_name.c_str());    // name of mapping object

    s_seg_counter++;
    if (d_handle == NULL || d_handle == INVALID_HANDLE_VALUE) {
        std::string msg =
            "gr::vmcircbuf_mmap_createfilemapping: CreateFileMapping [" + seg_name + "]";
        werror((char*)msg.c_str(), GetLastError());
        throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    // Allocate virtual memory of the needed size, then free it so we can use it
    LPVOID first_tmp;
    first_tmp = VirtualAlloc(NULL, 2 * size, MEM_RESERVE, PAGE_NOACCESS);
    if (first_tmp == NULL) {
        werror("gr::vmcircbuf_mmap_createfilemapping: VirtualAlloc", GetLastError());
        CloseHandle(d_handle); // cleanup
        throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    if (VirtualFree(first_tmp, 0, MEM_RELEASE) == 0) {
        werror("gr::vmcircbuf_mmap_createfilemapping: VirtualFree", GetLastError());
        CloseHandle(d_handle); // cleanup
        throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    d_first_copy = MapViewOfFileEx((HANDLE)d_handle, // handle to map object
                                   FILE_MAP_WRITE,   // read/write permission
                                   0,
                                   0,
                                   size,
                                   first_tmp);
    if (d_first_copy != first_tmp) {
        werror("gr::vmcircbuf_mmap_createfilemapping: MapViewOfFileEx(1)",
               GetLastError());
        CloseHandle(d_handle); // cleanup
        throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    d_second_copy = MapViewOfFileEx((HANDLE)d_handle, // handle to map object
                                    FILE_MAP_WRITE,   // read/write permission
                                    0,
                                    0,
                                    size,
                                    (char*)first_tmp +
                                        size); //(LPVOID) ((char *)d_first_copy + size));

    if (d_second_copy != (char*)first_tmp + size) {
        werror("gr::vmcircbuf_mmap_createfilemapping: MapViewOfFileEx(2)",
               GetLastError());
        UnmapViewOfFile(d_first_copy);
        CloseHandle(d_handle); // cleanup
        throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

#ifdef DEBUG
    d_debug_logger->info("contiguous? mmap {:s} {:s} {:d} {:s}",
                         (char*)d_first_copy,
                         (char*)d_second_copy,
                         size,
                         (char*)d_first_copy + size);
#endif

    // Now remember the important stuff
    d_base = (char*)d_first_copy;
    d_size = size;
#endif /*HAVE_CREATEFILEMAPPING*/
}

vmcircbuf_createfilemapping::~vmcircbuf_createfilemapping()
{
#ifdef HAVE_CREATEFILEMAPPING
    gr::thread::scoped_lock guard(s_vm_mutex);

    if (UnmapViewOfFile(d_first_copy) == 0) {
        werror("gr::vmcircbuf_createfilemapping: UnmapViewOfFile(d_first_copy)",
               GetLastError());
    }
    d_base = NULL;
    if (UnmapViewOfFile(d_second_copy) == 0) {
        werror("gr::vmcircbuf_createfilemapping: UnmapViewOfFile(d_second_copy)",
               GetLastError());
    }
    // d_second=NULL;
    CloseHandle(d_handle);
#endif
}

// ----------------------------------------------------------------
//      The factory interface
// ----------------------------------------------------------------

gr::vmcircbuf_factory* vmcircbuf_createfilemapping_factory::s_the_factory = 0;

gr::vmcircbuf_factory* vmcircbuf_createfilemapping_factory::singleton()
{
    if (s_the_factory)
        return s_the_factory;
    s_the_factory = new vmcircbuf_createfilemapping_factory();
    return s_the_factory;
}

int vmcircbuf_createfilemapping_factory::granularity()
{
#ifdef HAVE_CREATEFILEMAPPING
    //  return 65536;//TODO, check, is this needed or can we just use gr::pagesize()
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    // fprintf(stderr,"win32 AllocationGranularity
    // %p\n",(int)system_info.dwAllocationGranularity);
    return (int)system_info.dwAllocationGranularity;
#else
    return gr::pagesize();
#endif
}

gr::vmcircbuf* vmcircbuf_createfilemapping_factory::make(size_t size)
{
    try {
        return new vmcircbuf_createfilemapping(size);
    } catch (...) {
        return 0;
    }
}

} /* namespace gr */
