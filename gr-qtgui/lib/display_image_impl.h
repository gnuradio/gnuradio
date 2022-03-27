/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DISPLAY_IMAGE_IMPL_H
#define INCLUDED_DISPLAY_IMAGE_IMPL_H

#include <gnuradio/qtgui/display_image_sink.h>

#include <QApplication>

namespace gr {
namespace qtgui {

class QTGUI_API display_image_impl : public display_image
{
private:
    /* Variables */

    int d_width;
    int d_height;
    QWidget* d_parent;

    bool d_triggered;
    ShowPngPicture* d_main_gui;


public:
    display_image_impl(int imagewidth, int imageheight, QWidget* parent);
    ~display_image_impl();

    void displayBottomUp(bool direction);

    QApplication* d_qApplication;

    void exec_() override;
    QWidget* qwidget() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_DISPLAY_IMAGE_IMPL_H */
