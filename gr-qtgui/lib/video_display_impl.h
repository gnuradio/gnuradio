/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_VIDEO_DISPLAY_IMPL_H
#define INCLUDED_QTGUI_VIDEO_DISPLAY_IMPL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "video_player.h"
#include <gnuradio/qtgui/video_display.h>
#include <QtCore/QTemporaryFile>
#include <qapplication.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>

namespace gr {
namespace qtgui {

class video_display_impl : public video_display
{
private:
    const size_t d_itemsize;
    FILE* d_fp;
    const char* d_argv = "";
    int d_argc = 1;
    const char* d_filename;
    video_player* d_video_player;
    QWidget* d_parent;
    video_player_signal* d_video_player_signal;
    QTemporaryFile d_temp_file;

public:
    video_display_impl(size_t itemsize, QWidget* parent = nullptr);
    ~video_display_impl();

    void exec_() override;
    QApplication* d_qApplication;
    QWidget* qwidget() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_qtgui_VIDEO_DISPLAY_IMPL_H */
