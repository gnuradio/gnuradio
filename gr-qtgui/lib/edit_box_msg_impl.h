/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_EDIT_BOX_MSG_IMPL_H
#define INCLUDED_QTGUI_EDIT_BOX_MSG_IMPL_H

#include <gnuradio/qtgui/edit_box_msg.h>

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

namespace gr {
namespace qtgui {

class QTGUI_API edit_box_msg_impl : public QObject, public edit_box_msg
{
    Q_OBJECT

private:
    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;

    data_type_t d_type;
    bool d_is_pair;
    bool d_is_static;

    QGroupBox* d_group;
    QVBoxLayout* d_vlayout;
    QHBoxLayout* d_hlayout;
    QLabel* d_label;
    QLineEdit* d_val;
    QLineEdit* d_key;
    QComboBox* d_type_box;

    pmt::pmt_t d_msg;
    const pmt::pmt_t d_port;

public:
    edit_box_msg_impl(gr::qtgui::data_type_t type,
                      const std::string& value = "",
                      const std::string& label = "",
                      bool is_pair = false,
                      bool is_static = true,
                      const std::string& key = "",
                      QWidget* parent = 0);
    ~edit_box_msg_impl() override;

    // Overload the start method of gr::block to emit a message if a
    // default value is provided.
    bool start() override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_value(pmt::pmt_t val);

public slots:
    void edit_finished();
    void set_type(int);
    void set_type(gr::qtgui::data_type_t type);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_EDIT_BOX_MSG_IMPL_H */
