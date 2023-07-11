/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "video_display_impl.h"
#include <gnuradio/io_signature.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>

#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define OUR_O_BINARY O_BINARY
#else
#define OUR_O_BINARY 0
#endif

#ifdef O_LARGEFILE
#define OUR_O_LARGEFILE O_LARGEFILE
#else
#define OUR_O_LARGEFILE 0
#endif

namespace gr {
namespace qtgui {

video_display::sptr video_display::make(size_t itemsize, QWidget* parent)
{
    return gnuradio::make_block_sptr<video_display_impl>(itemsize, parent);
}

video_display_impl::video_display_impl(size_t itemsize, QWidget* parent)
    : gr::sync_block("video_display",
                     gr::io_signature::make(1, 1, itemsize),
                     gr::io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_parent(parent),
      d_video_player_signal(new video_player_signal()),
      d_qApplication(qApp ? qApp : new QApplication(d_argc, const_cast<char**>(&d_argv)))
{
    d_temp_file.setAutoRemove(false);
    d_temp_file.setFileTemplate("video_display_XXXXXX.mp4");
    std::string temporary_file = d_temp_file.fileName().toStdString();
    d_filename = temporary_file.data();

    d_fp = std::fopen(d_filename, "ab+");
    if (!d_fp) {
        qWarning() << "Could not open file " << d_filename;
        return;
    }

    d_video_player = new video_player(d_parent, d_filename);

    QObject::connect(d_video_player_signal,
                     &video_player_signal::data_send,
                     d_video_player,
                     &video_player::data_read);
}

video_display_impl::~video_display_impl()
{
    if (d_fp) {
        fclose(d_fp);
    }
    std::string temporary_file = d_temp_file.fileName().toStdString();
    d_filename = temporary_file.data();

    QFile::remove(d_filename);
}

void video_display_impl::exec_() { d_qApplication->exec(); }

QWidget* video_display_impl::qwidget() { return (QWidget*)d_video_player; }

int video_display_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const char* inbuf = static_cast<const char*>(input_items[0]);

    int nwritten = 0;

    if (!d_fp)
        return noutput_items;

    while (nwritten < noutput_items) {
        const int count = fwrite(inbuf, d_itemsize, noutput_items - nwritten, d_fp);
        if (count == 0) {
            if (ferror(d_fp)) {
                std::stringstream s;
                s << "file_sink write failed with error " << fileno(d_fp) << std::endl;
                throw std::runtime_error(s.str());
            } else {
                break;
            }
        }
        nwritten += count;
        inbuf += count * d_itemsize;
    }

    fflush(d_fp);

    if (inbuf != nullptr && inbuf[0] != '\0') {
        emit d_video_player_signal->data_send();
    }

    return noutput_items;
}

} /* namespace qtgui */
} /* namespace gr */
