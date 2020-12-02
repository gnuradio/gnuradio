/* -*- c++ -*- */
/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_source_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <stdexcept>

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

namespace gr {
namespace blocks {

file_source::sptr file_source::make(size_t itemsize,
                                    const char* filename,
                                    bool repeat,
                                    uint64_t start_offset_items,
                                    uint64_t length_items)
{
    return gnuradio::get_initial_sptr(new file_source_impl(
        itemsize, filename, repeat, start_offset_items, length_items));
}

file_source_impl::file_source_impl(size_t itemsize,
                                   const char* filename,
                                   bool repeat,
                                   uint64_t start_offset_items,
                                   uint64_t length_items)
    : sync_block(
          "file_source", io_signature::make(0, 0, 0), io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_start_offset_items(start_offset_items),
      d_length_items(length_items),
      d_fp(0),
      d_new_fp(0),
      d_repeat(repeat),
      d_updated(false),
      d_file_begin(true),
      d_repeat_cnt(0),
      d_add_begin_tag(pmt::PMT_NIL)
{
    open(filename, repeat, start_offset_items, length_items);
    do_update();

    std::stringstream str;
    str << name() << unique_id();
    _id = pmt::string_to_symbol(str.str());
}

file_source_impl::~file_source_impl()
{
    if (d_fp)
        fclose((FILE*)d_fp);
    if (d_new_fp)
        fclose((FILE*)d_new_fp);
}

bool file_source_impl::seek(int64_t seek_point, int whence)
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
            GR_LOG_WARN(d_logger, "bad seek mode");
            return 0;
        }

        if ((seek_point < (int64_t)d_start_offset_items) ||
            (seek_point > (int64_t)(d_start_offset_items + d_length_items - 1))) {
            GR_LOG_WARN(d_logger, "bad seek point");
            return 0;
        }
        return GR_FSEEK((FILE*)d_fp, seek_point * d_itemsize, SEEK_SET) == 0;
    } else {
        GR_LOG_WARN(d_logger, "file not seekable");
        return 0;
    }
}


void file_source_impl::open(const char* filename,
                            bool repeat,
                            uint64_t start_offset_items,
                            uint64_t length_items)
{
    // obtain exclusive access for duration of this function
    gr::thread::scoped_lock lock(fp_mutex);

    if (d_new_fp) {
        fclose(d_new_fp);
        d_new_fp = 0;
    }

    if ((d_new_fp = fopen(filename, "rb")) == NULL) {
        GR_LOG_ERROR(d_logger, boost::format("%s: %s") % filename % strerror(errno));
        throw std::runtime_error("can't open file");
    }

    struct GR_STAT st;

    if (GR_FSTAT(GR_FILENO(d_new_fp), &st)) {
        GR_LOG_ERROR(d_logger, boost::format("%s: %s") % filename % strerror(errno));
        throw std::runtime_error("can't fstat file");
    }
    if (S_ISREG(st.st_mode)) {
        d_seekable = true;
    } else {
        d_seekable = false;
    }

    uint64_t file_size;

    if (d_seekable) {
        // Check to ensure the file will be consumed according to item size
        GR_FSEEK(d_new_fp, 0, SEEK_END);
        file_size = GR_FTELL(d_new_fp);

        // Make sure there will be at least one item available
        if ((file_size / d_itemsize) < (start_offset_items + 1)) {
            if (start_offset_items) {
                GR_LOG_WARN(d_logger, "file is too small for start offset");
            } else {
                GR_LOG_WARN(d_logger, "file is too small");
            }
            fclose(d_new_fp);
            throw std::runtime_error("file is too small");
        }
    } else {
        file_size = INT64_MAX;
    }

    uint64_t items_available = (file_size / d_itemsize - start_offset_items);

    // If length is not specified, use the remainder of the file. Check alignment at end.
    if (length_items == 0) {
        length_items = items_available;
        if (file_size % d_itemsize) {
            GR_LOG_WARN(d_logger, "file size is not a multiple of item size");
        }
    }

    // Check specified length. Warn and use available items instead of throwing an
    // exception.
    if (length_items > items_available) {
        length_items = items_available;
        GR_LOG_WARN(d_logger, "file too short, will read fewer than requested items");
    }

    // Rewind to start offset
    if (d_seekable) {
        GR_FSEEK(d_new_fp, start_offset_items * d_itemsize, SEEK_SET);
    }

    d_updated = true;
    d_repeat = repeat;
    d_start_offset_items = start_offset_items;
    d_length_items = length_items;
    d_items_remaining = length_items;
}

void file_source_impl::close()
{
    // obtain exclusive access for duration of this function
    gr::thread::scoped_lock lock(fp_mutex);

    if (d_new_fp != NULL) {
        fclose(d_new_fp);
        d_new_fp = NULL;
    }
    d_updated = true;
}

void file_source_impl::do_update()
{
    if (d_updated) {
        gr::thread::scoped_lock lock(fp_mutex); // hold while in scope

        if (d_fp)
            fclose(d_fp);

        d_fp = d_new_fp; // install new file pointer
        d_new_fp = 0;
        d_updated = false;
        d_file_begin = true;
    }
}

void file_source_impl::set_begin_tag(pmt::pmt_t val) { d_add_begin_tag = val; }

int file_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    char* o = (char*)output_items[0];
    uint64_t size = noutput_items;

    do_update(); // update d_fp is reqd
    if (d_fp == NULL)
        throw std::runtime_error("work with file not open");

    gr::thread::scoped_lock lock(fp_mutex); // hold for the rest of this function

    // No items remaining - all done
    if (d_items_remaining == 0)
        return WORK_DONE;

    while (size) {

        // Add stream tag whenever the file starts again
        if (d_file_begin && d_add_begin_tag != pmt::PMT_NIL) {
            add_item_tag(0,
                         nitems_written(0) + noutput_items - size,
                         d_add_begin_tag,
                         pmt::from_long(d_repeat_cnt),
                         _id);
            d_file_begin = false;
        }

        uint64_t nitems_to_read = std::min(size, d_items_remaining);

        // Since the bounds of the file are known, unexpected nitems is an error
        if (nitems_to_read != fread(o, d_itemsize, nitems_to_read, (FILE*)d_fp))
            throw std::runtime_error("fread error");

        size -= nitems_to_read;
        d_items_remaining -= nitems_to_read;
        o += nitems_to_read * d_itemsize;

        // Ran out of items ("EOF")
        if (d_items_remaining == 0) {

            // Repeat: rewind and request tag
            if (d_repeat && d_seekable) {
                GR_FSEEK(d_fp, d_start_offset_items * d_itemsize, SEEK_SET);
                d_items_remaining = d_length_items;
                if (d_add_begin_tag != pmt::PMT_NIL) {
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

    return (noutput_items - size);
}

} /* namespace blocks */
} /* namespace gr */
