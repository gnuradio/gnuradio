/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_DISPLAY_TEXT_SINK_IMPL_H
#define INCLUDED_DISPLAY_TEXT_SINK_IMPL_H

#include <gnuradio/qtgui/display_text_sink.h>

#include <QApplication>

class display_text_window;

namespace gr {
namespace qtgui {

class QTGUI_API display_text_sink_impl : public display_text_sink
{
private:
    int d_splitlength;
    QWidget* d_parent;
    display_text_window* d_main_gui;

public:
    display_text_sink_impl(const std::string& label,
                           int splitlength,
                           int maxlines,
                           QWidget* parent);
    ~display_text_sink_impl();

    QApplication* d_qApplication;

    void exec_() override;
    QWidget* qwidget() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_DISPLAY_TEXT_SINK_IMPL_H */
