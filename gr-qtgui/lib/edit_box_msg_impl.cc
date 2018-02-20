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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "edit_box_msg_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <gnuradio/qtgui/utils.h>
#include <boost/lexical_cast.hpp>

namespace gr {
  namespace qtgui {

    edit_box_msg::sptr
    edit_box_msg::make(data_type_t type, const std::string &label,
                       const std::string &value,
                       bool is_pair, bool is_static,
                       const std::string &key, QWidget* parent)
    {
      return gnuradio::get_initial_sptr
        (new edit_box_msg_impl(type, value, label, is_pair,
                               is_static, key, parent));
    }

    edit_box_msg_impl::edit_box_msg_impl(data_type_t type, const std::string &label,
                                         const std::string &value,
                                         bool is_pair, bool is_static,
                                         const std::string &key, QWidget* parent)
      : block("edit_box_msg",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0)),
        QObject(parent),
        d_port(pmt::mp("msg"))
    {
      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
#if QT_VERSION >= 0x040500
        std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
        QApplication::setGraphicsSystem(QString(style.c_str()));
#endif
	d_qApplication = new QApplication(d_argc, &d_argv);
      }

      // If a style sheet is set in the prefs file, enable it here.
      check_set_qss(d_qApplication);

      d_is_pair = is_pair;
      d_is_static = is_static;

      d_val = new QLineEdit();
      d_val->setObjectName("qtgui_editboxmsg_val"); // used to set background color
      d_val->setText(QString(value.c_str()));

      set_type(type);

      d_group = new QGroupBox();
      d_vlayout = new QVBoxLayout(parent);
      d_hlayout = new QHBoxLayout(parent);

      if(d_is_pair) {
        d_key = new QLineEdit();

        QString key_text = QString(key.c_str());
        d_key->setText(key_text);

        // If static, we create the d_key object, which we use later
        // to be consistent about getting the key string. But we do
        // not add it to the layout.
        if(d_is_static) {
          d_key->setEnabled(false);

          QFontMetrics fm = d_key->fontMetrics();
          int width = 15 + fm.width(key_text);

          d_key->setFixedWidth(width);

          // Verify that a default key has been set or emit an error
          if(key.size() == 0) {
            throw std::runtime_error("When using static + pair mode, please set a default key.");
          }
        }
        else {
          // Adding it to the layout if in non-static mode so users
          // can see and update the key.
          d_hlayout->addWidget(d_key);
        }
      }

      d_label = NULL;
      if(label != "") {
        d_label = new QLabel(QString(label.c_str()));
        d_vlayout->addWidget(d_label);
      }

      d_hlayout->addWidget(d_val);

      if(!d_is_static) {
        // If not static, we can change the key and the data type of
        // the value box.
        d_type_box = new QComboBox();
        d_type_box->setEditable(false);

        // Items listed in order of enum data_type_t
        d_type_box->addItem("Int");
        d_type_box->addItem("Float");
        d_type_box->addItem("Double");
        d_type_box->addItem("Complex");
        d_type_box->addItem("String");
        d_type_box->addItem("Int (vec)");
        d_type_box->addItem("Float (vec)");
        d_type_box->addItem("Double (vec)");
        d_type_box->addItem("Complex (vec)");
        d_type_box->setCurrentIndex(d_type);
        d_hlayout->addWidget(d_type_box);

        QObject::connect(d_type_box, SIGNAL(currentIndexChanged(int)),
                         this, SLOT(set_type(int)));
      }

      d_vlayout->addItem(d_hlayout);
      d_group->setLayout(d_vlayout);

      QObject::connect(d_val, SIGNAL(editingFinished()),
                       this, SLOT(edit_finished()));

      d_msg = pmt::PMT_NIL;

      message_port_register_out(d_port);
      message_port_register_in(pmt::mp("val"));

      set_msg_handler(pmt::mp("val"),
                      boost::bind(&edit_box_msg_impl::set_value, this, _1));
    }

    edit_box_msg_impl::~edit_box_msg_impl()
    {
      delete d_argv;
      delete d_group;
      delete d_hlayout;
      delete d_vlayout;
      delete d_val;
      if(d_is_pair)
        delete d_key;
      if(d_label)
        delete d_label;
    }

    bool
    edit_box_msg_impl::start()
    {
      QString text = d_val->text();
      if(!text.isEmpty()) {
        edit_finished();
      }

      return block::start();
    }

    void
    edit_box_msg_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    edit_box_msg_impl::qwidget()
    {
      return (QWidget*)d_group;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    edit_box_msg_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_group);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    edit_box_msg_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    edit_box_msg_impl::set_type(int type)
    {
      set_type(static_cast<data_type_t>(type));
    }

    void
    edit_box_msg_impl::set_type(gr::qtgui::data_type_t type)
    {
      d_type = type;

      switch(d_type) {
      case INT:
      case INT_VEC:
        d_val->setStyleSheet("QLineEdit#qtgui_editboxmsg_val {background-color: #4CAF50;}");
        break;
      case FLOAT:
      case FLOAT_VEC:
        d_val->setStyleSheet("QLineEdit#qtgui_editboxmsg_val {background-color: #F57C00;}");
        break;
      case DOUBLE:
      case DOUBLE_VEC:
        d_val->setStyleSheet("QLineEdit#qtgui_editboxmsg_val {background-color: #00BCD4;}");
        break;
      case COMPLEX:
      case COMPLEX_VEC:
        d_val->setStyleSheet("QLineEdit#qtgui_editboxmsg_val {background-color: #2196F3;}");
        break;
      case STRING:
        d_val->setStyleSheet("QLineEdit#qtgui_editboxmsg_val {background-color: #FFFFFF; color: #000000;}");
        break;
      }
    }

    void
    edit_box_msg_impl::set_value(pmt::pmt_t val)
    {
      // If the contents of the new value are the same as we already
      // had, don't update anything, just exit and move on.
      if(pmt::eqv(val, d_msg)) {
        return;
      }

      int xi;
      float xf;
      double xd;
      std::string xs;
      gr_complex xc;

      d_msg = val;

      // Only update key if we're expecting a pair
      if(d_is_pair) {
        // If we are, make sure that the PMT is actually a pair
        if(pmt::is_pair(val)) {
          pmt::pmt_t key = pmt::car(val);
          std::string skey = pmt::symbol_to_string(key);

          // If static, check to make sure that the key of the
          // incoming message matches our key. If it doesn't, emit a
          // warning and exit without changing anything.
          if(d_is_static) {
            std::string cur_key = d_key->text().toStdString();
            if(skey != cur_key) {
              GR_LOG_WARN(d_logger, boost::format("Got key '%1%' but expected '%2%'") \
                          % skey % cur_key);
              return;
            }
          }
          val = pmt::cdr(val);
          d_key->setText(QString(skey.c_str()));
        }
        else {
          GR_LOG_WARN(d_logger, "Did not find PMT pair");
          return;
        }
      }

      switch(d_type) {
      case INT:
        if(pmt::is_integer(val)) {
          xi = pmt::to_long(val);
          d_val->setText(QString::number(xi));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from integer failed");
          return;
        }
        break;
      case INT_VEC:
        if(pmt::is_s32vector(val)) {
          QStringList text_list;
          const std::vector<int32_t> xv = pmt::s32vector_elements(val);
          for(size_t i = 0; i < xv.size(); i++) {
            text_list.append(QString::number(xv[i]));
          }
          d_val->setText(text_list.join(", "));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from integer vector failed");
          return;
        }
        break;
      case FLOAT:
        if(pmt::is_real(val)) {
          xf = pmt::to_float(val);
          d_val->setText(QString::number(xf));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from float failed");
          return;
        }
        break;
      case FLOAT_VEC:
        if(pmt::is_f32vector(val)) {
          QStringList text_list;
          const std::vector<float> xv = pmt::f32vector_elements(val);
          for(size_t i = 0; i < xv.size(); i++) {
            text_list.append(QString::number(xv[i]));
          }
          d_val->setText(text_list.join(", "));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from float vector failed");
          return;
        }
        break;
      case DOUBLE:
        if(pmt::is_real(val)) {
          xd = pmt::to_double(val);
          d_val->setText(QString::number(xd));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from double failed");
          return;
        }
        break;
      case DOUBLE_VEC:
        if(pmt::is_f64vector(val)) {
          QStringList text_list;
          const std::vector<double> xv = pmt::f64vector_elements(val);
          for(size_t i = 0; i < xv.size(); i++) {
            text_list.append(QString::number(xv[i]));
          }
          d_val->setText(text_list.join(", "));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from double vector failed");
          return;
        }
        break;
      case COMPLEX:
        if(pmt::is_complex(val)) {
          xc = pmt::to_complex(val);
          d_val->setText(QString("(%1,%2)").arg(xc.real()).arg(xc.imag()));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from complex failed");
          return;
        }
        break;
      case COMPLEX_VEC:
        if(pmt::is_c32vector(val)) {
          QStringList text_list;
          const std::vector<gr_complex> xv = pmt::c32vector_elements(val);
          for(size_t i = 0; i < xv.size(); i++) {
            text_list.append(QString("(%1,%2)").arg(xv[i].real()).arg(xv[i].imag()));
          }
          d_val->setText(text_list.join(", "));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from complex vector failed");
          return;
        }
        break;
      case STRING:
        if(pmt::is_symbol(val)) {
          xs = pmt::symbol_to_string(val);
          d_val->setText(QString(xs.c_str()));
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion from string failed");
          return;
        }
        break;
      }

      // Emit the new message to pass updates downstream.
      // Loops are prevented by the early exit if d_msg == val.
      message_port_pub(d_port, d_msg);
    }

    void
    edit_box_msg_impl::edit_finished()
    {
      QString text = d_val->text();
      bool conv_ok = true;
      int xi;
      float xf;
      double xd;
      std::string xs;
      gr_complex xc;

      switch(d_type) {
      case INT:
        xi = text.toInt(&conv_ok);
        if(conv_ok) {
          d_msg = pmt::from_long(xi);
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion to integer failed");
          return;
        }
        break;
      case INT_VEC:
        {
          std::vector<int32_t> xv;
          QStringList text_list = text.split(",");
          for(int i = 0; i < text_list.size(); ++i) {
            QString s = text_list.at(i);
            s = s.remove(QChar(' '));
            int t = s.toInt(&conv_ok);
            if(conv_ok) {
              xv.push_back(t);
            }
            else {
              GR_LOG_WARN(d_logger, "Conversion to integer vector failed");
              return;
            }
          }
          d_msg = pmt::init_s32vector(xv.size(), xv);
        }
        break;
      case FLOAT:
        xf = text.toFloat(&conv_ok);
        if(conv_ok) {
          d_msg = pmt::from_float(xf);
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion to float failed");
          return;
        }
        break;
      case FLOAT_VEC:
        {
          std::vector<float> xv;
          QStringList text_list = text.split(",");
          for(int i = 0; i < text_list.size(); ++i) {
            QString s = text_list.at(i);
            s = s.remove(QChar(' '));
            float t = s.toFloat(&conv_ok);
            if(conv_ok) {
              xv.push_back(t);
            }
            else {
              GR_LOG_WARN(d_logger, "Conversion to float vector failed");
              return;
            }
          }
          d_msg = pmt::init_f32vector(xv.size(), xv);
        }
        break;
      case DOUBLE:
        xd = text.toDouble(&conv_ok);
        if(conv_ok) {
          d_msg = pmt::from_double(xd);
        }
        else {
          GR_LOG_WARN(d_logger, "Conversion to double failed");
          return;
        }
        break;
      case DOUBLE_VEC:
        {
          std::vector<double> xv;
          QStringList text_list = text.split(",");
          for(int i = 0; i < text_list.size(); ++i) {
            QString s = text_list.at(i);
            s = s.remove(QChar(' '));
            double t = s.toDouble(&conv_ok);
            if(conv_ok) {
              xv.push_back(t);
            }
            else {
              GR_LOG_WARN(d_logger, "Conversion to double vector failed");
              return;
            }
          }
          d_msg = pmt::init_f64vector(xv.size(), xv);
        }
        break;
      case COMPLEX:
        try {
          xc = boost::lexical_cast<gr_complex>(text.toStdString());
        }
        catch(boost::bad_lexical_cast const & e) {
          GR_LOG_WARN(d_logger, boost::format("Conversion to complex failed (%1%)") \
                      % e.what());
          return;
        }
        d_msg = pmt::from_complex(xc.real(), xc.imag());
        break;
      case COMPLEX_VEC:
        {
          std::vector<gr_complex> xv;
          QStringList text_list = text.split(",");
          bool even = false;
          gr_complex c;
          float re, im;
          for(int i = 0; i < text_list.size(); ++i) {
            QString s = text_list.at(i);
            s = s.remove(QChar(' '));
            s = s.remove(QChar(')'));
            s = s.remove(QChar('('));
            float t = s.toFloat(&conv_ok);
            if(conv_ok) {
              if(even) {
                im = t;
                xv.push_back(gr_complex(re, im));
                even = false;
              }
              else {
                re = t;
                even = true;
              }
            }
            else {
              GR_LOG_WARN(d_logger, "Conversion to complex vector failed");
              return;
            }
          }
          d_msg = pmt::init_c32vector(xv.size(), xv);
        }
        break;
      case STRING:
        xs = text.toStdString();
        d_msg = pmt::intern(xs);
        break;
      }

      if(d_is_pair) {
        std::string key = d_key->text().toStdString();
        d_msg = pmt::cons(pmt::intern(key), d_msg);
      }

      message_port_pub(d_port, d_msg);
    }

  } /* namespace qtgui */
} /* namespace gr */
