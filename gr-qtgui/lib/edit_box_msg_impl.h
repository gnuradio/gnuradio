/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_EDIT_BOX_MSG_IMPL_H
#define INCLUDED_QTGUI_EDIT_BOX_MSG_IMPL_H

#include <gnuradio/qtgui/edit_box_msg.h>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

namespace gr {
  namespace qtgui {

    class QTGUI_API edit_box_msg_impl
      : public QObject, public edit_box_msg
    {
      Q_OBJECT

    private:
      int d_argc;
      char *d_argv;
      data_type_t d_type;
      bool d_is_pair;
      bool d_is_static;

      QGroupBox *d_group;
      QVBoxLayout *d_vlayout;
      QHBoxLayout *d_hlayout;
      QLabel *d_label;
      QLineEdit *d_val;
      QLineEdit *d_key;
      QComboBox *d_type_box;

      pmt::pmt_t d_msg;
      const pmt::pmt_t d_port;

    public:
      edit_box_msg_impl(gr::qtgui::data_type_t type,
                        const std::string &value="",
                        const std::string &label="",
                        bool is_pair=false,
                        bool is_static=true,
                        const std::string &key="",
                        QWidget* parent=0);
      ~edit_box_msg_impl();

      // Overload the start method of gr::block to emit a message if a
      // default value is provided.
      bool start();

      void exec_();
      QWidget*  qwidget();

#ifdef ENABLE_PYTHON
      PyObject* pyqwidget();
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
