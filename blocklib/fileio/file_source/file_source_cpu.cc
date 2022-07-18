/* -*- c++ -*- */
/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "file_source_cpu.h"
#include "file_source_cpu_gen.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <filesystem>
#include <stdexcept>

#include <pmtf/scalar.hpp>
#include <pmtf/string.hpp>

#ifdef _MSC_VER
#define GR_FSEEK _fseeki64
#define GR_FTELL _ftelli64
#define GR_FSTAT _fstati64
#define GR_FILENO _fileno
#define GR_STAT _stati64
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#else
#define GR_FSEEK fseeko
#define GR_FTELL ftello
#define GR_FSTAT fstat
#define GR_FILENO fileno
#define GR_STAT stat
#endif

namespace fs = std::filesystem;

namespace gr {
namespace fileio {

file_source_cpu::file_source_cpu(const file_source::block_args& args)
    : INHERITED_CONSTRUCTORS,

      d_start_offset_items(args.offset),
      d_length_items(args.len),
      d_repeat(args.repeat),
      d_itemsize(args.itemsize),
      d_filename(args.filename),
      d_offset(args.offset)
{

    if (args.itemsize > 0) {
        open(d_filename, d_repeat, d_offset, d_length_items);
        do_update();
    }
    else {
        fs::path f{ d_filename };
        if (!fs::exists(f)) {
            throw new std::runtime_error(
                fmt::format("file_source: the file {} does not exist", d_filename));
        }
    }

    std::stringstream str;
    str << name() << id();
    _id = pmtf::string(str.str());
}


file_source_cpu::~file_source_cpu()
{
    if (d_fp)
        fclose((FILE*)d_fp);
    if (d_new_fp)
        fclose((FILE*)d_new_fp);
}

bool file_source_cpu::seek(int64_t seek_point, int whence)
{
    if (d_seekable) {
        seek_point += d_start_offset_items;

        switch (whence) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            seek_point += (d_length_items - d_items_remaining);
            break;
        case SEEK_END:
            seek_point = d_length_items - seek_point;
            break;
        default:
            d_logger->warn("bad seek mode");
            return 0;
        }

        if ((seek_point < (int64_t)d_start_offset_items) ||
            (seek_point > (int64_t)(d_start_offset_items + d_length_items - 1))) {
            d_logger->warn("bad seek point");
            return 0;
        }
        return GR_FSEEK((FILE*)d_fp, seek_point * d_itemsize, SEEK_SET) == 0;
    }
    else {
        d_logger->warn("file not seekable");
        return 0;
    }
}


void file_source_cpu::open(const std::string& filename,
                           bool repeat,
                           uint64_t start_offset_items,
                           uint64_t length_items)
{
    // obtain exclusive access for duration of this function
    std::scoped_lock lock(fp_mutex);

    if (d_new_fp) {
        fclose(d_new_fp);
        d_new_fp = 0;
    }

    if ((d_new_fp = fopen(filename.c_str(), "rb")) == NULL) {
        d_logger->error("{}:{}", filename, strerror(errno));
        throw std::runtime_error("can't open file");
    }

    struct GR_STAT st;

    if (GR_FSTAT(GR_FILENO(d_new_fp), &st)) {
        d_logger->error("{}:{}", filename, strerror(errno));
        throw std::runtime_error("can't fstat file");
    }
    if (S_ISREG(st.st_mode)) {
        d_seekable = true;
    }
    else {
        d_seekable = false;
    }

    uint64_t file_size;

    if (d_seekable) {
        // Check to ensure the file will be consumed according to item size
        if (GR_FSEEK(d_new_fp, 0, SEEK_END) == -1) {
            throw std::runtime_error("can't fseek()");
        }
        file_size = GR_FTELL(d_new_fp);

        // Make sure there will be at least one item available
        if ((file_size / d_itemsize) < (start_offset_items + 1)) {
            if (start_offset_items) {
                d_logger->warn("file is too small for start offset");
            }
            else {
                d_logger->warn("file is too small");
            }
            fclose(d_new_fp);
            throw std::runtime_error("file is too small");
        }
    }
    else {
        file_size = INT64_MAX;
    }

    uint64_t items_available = (file_size / d_itemsize - start_offset_items);

    // If length is not specified, use the remainder of the file. Check alignment at end.
    if (length_items == 0) {
        length_items = items_available;
        if (file_size % d_itemsize) {
            d_logger->warn("file size is not a multiple of item size");
        }
    }

    // Check specified length. Warn and use available items instead of throwing an
    // exception.
    if (length_items > items_available) {
        length_items = items_available;
        d_logger->warn("file too short, will read fewer than requested items");
    }

    // Rewind to start offset
    if (d_seekable) {
        auto start_offset = start_offset_items * d_itemsize;
#ifdef _POSIX_C_SOURCE
#if _POSIX_C_SOURCE >= 200112L
        // If supported, tell the OS that we'll be accessing the file sequentially
        // and that it would be a good idea to start prefetching it
        auto fd = fileno(d_new_fp);
        static const std::map<int, const std::string> fadv_errstrings = {
            { EBADF, "bad file descriptor" },
            { EINVAL, "invalid advise" },
            { ESPIPE, "tried to act as if a pipe or similar was a file" }
        };
        if (file_size && file_size != INT64_MAX) {
            if (auto ret = posix_fadvise(
                    fd, start_offset, file_size - start_offset, POSIX_FADV_SEQUENTIAL)) {
                d_logger->warn("failed to advise to read sequentially, " +
                               fadv_errstrings.at(ret));
            }
            if (auto ret = posix_fadvise(
                    fd, start_offset, file_size - start_offset, POSIX_FADV_WILLNEED)) {
                d_logger->warn("failed to advise we'll need file contents soon, " +
                               fadv_errstrings.at(ret));
            }
        }
#endif
#endif
        if (GR_FSEEK(d_new_fp, start_offset, SEEK_SET) == -1) {
            throw std::runtime_error("can't fseek()");
        }
    }

    d_updated = true;
    d_repeat = repeat;
    d_start_offset_items = start_offset_items;
    d_length_items = length_items;
    d_items_remaining = length_items;
}

void file_source_cpu::close()
{
    // obtain exclusive access for duration of this function
    std::scoped_lock lock(fp_mutex);

    if (d_new_fp != NULL) {
        fclose(d_new_fp);
        d_new_fp = NULL;
    }
    d_updated = true;
}

void file_source_cpu::do_update()
{
    if (d_updated) {
        std::scoped_lock lock(fp_mutex); // hold while in scope

        if (d_fp)
            fclose(d_fp);

        d_fp = d_new_fp; // install new file pointer
        d_new_fp = 0;
        d_updated = false;
        d_file_begin = true;
    }
}

void file_source_cpu::set_begin_tag(const std::string& val) { d_add_begin_tag = val; }

work_return_code_t file_source_cpu::work(work_io& wio)
{
    auto out = wio.outputs()[0].items<uint8_t>();
    auto noutput_items = wio.outputs()[0].n_items;
    uint64_t size = noutput_items;

    if (d_itemsize == 0) {
        d_itemsize = wio.outputs()[0].buf().item_size();
        open(d_filename, d_repeat, d_offset, d_length_items);
    }

    do_update(); // update d_fp is reqd
    if (d_fp == NULL)
        throw std::runtime_error("work with file not open");

    std::scoped_lock lock(fp_mutex); // hold for the rest of this function

    // No items remaining - all done
    if (d_items_remaining == 0) {
        wio.outputs()[0].n_produced = 0;
        return work_return_code_t::WORK_DONE;
    }

    while (size) {

        // Add stream tag whenever the file starts again
        if (d_file_begin && !d_add_begin_tag.empty()) {
            wio.outputs()[0].buf().add_tag(
                wio.outputs()[0].buf().total_written() + noutput_items - size,
                pmtf::map{ { d_add_begin_tag, pmtf::scalar<int64_t>(d_repeat_cnt) },
                           { "srcid", _id } });

            d_file_begin = false;
        }

        uint64_t nitems_to_read = std::min(size, d_items_remaining);

        size_t nitems_read = fread(out, d_itemsize, nitems_to_read, (FILE*)d_fp);
        if (nitems_to_read != nitems_read) {
            // Size of non-seekable files is unknown. EOF is normal.
            if (!d_seekable && feof((FILE*)d_fp)) {
                size -= nitems_read;
                d_items_remaining = 0;
                break;
            }

            throw std::runtime_error("fread error");
        }

        size -= nitems_read;
        d_items_remaining -= nitems_read;
        out += nitems_read * d_itemsize;

        // Ran out of items ("EOF")
        if (d_items_remaining == 0) {
            // Repeat: rewind and request tag
            if (d_repeat && d_seekable) {
                if (GR_FSEEK(d_fp, d_start_offset_items * d_itemsize, SEEK_SET) == -1) {
                    throw std::runtime_error("can't fseek()");
                }
                d_items_remaining = d_length_items;
                if (!d_add_begin_tag.empty()) {
                    d_file_begin = true;
                    d_repeat_cnt++;
                }
            }

            // No repeat: return
            else {
                break;
            }
        }
    }

    wio.produce_each(noutput_items - size);
    return work_return_code_t::WORK_OK;
}


} // namespace fileio
} // namespace gr
