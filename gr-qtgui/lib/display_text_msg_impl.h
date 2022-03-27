/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DISPLAY_TEXT_MSG_IMPL_H
#define INCLUDED_DISPLAY_TEXT_MSG_IMPL_H

#include "display_text_window.h"
#include <gnuradio/qtgui/display_text_msg.h>
#include <qapplication.h>

namespace gr {
namespace qtgui {

class QTGUI_API display_text_msg_impl : public display_text_msg
{

private:
    int d_argc;
    char* d_argv;
    display_text_window* d_text;

public:
    display_text_msg_impl(const std::string& label,
                          const std::string& message_key,
                          int splitlength,
                          int maxlines,
                          QWidget* parent);
    ~display_text_msg_impl();

    void exec_() override;
    QApplication* d_qApplication;
    QWidget* qwidget() override;

    void set_value(pmt::pmt_t val);

private:
    pmt::pmt_t d_message_key;
    int d_splitlength;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_DISPLAY_TEXT_MSG_IMPL_H */
