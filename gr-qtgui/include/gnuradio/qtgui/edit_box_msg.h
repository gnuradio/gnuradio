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

#ifndef INCLUDED_QTGUI_EDIT_BOX_MSG_H
#define INCLUDED_QTGUI_EDIT_BOX_MSG_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/qtgui_types.h>
#include <gnuradio/qtgui/api.h>
#include <gnuradio/block.h>
#include <qapplication.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief Create a QT Edit Box widget where the value is posted as a message.
     * \ingroup qtgui_blk
     *
     * \details
     * This block creates a QT Edit Box widget that manages data
     * through message passing interfaces. The 'msg' output port
     * produces messages based on the text in the edit box and the
     * data type set by the \p type argument (see
     * gr::qtgui::data_type_t). The data types are checked, and WARN
     * log messages are produced when the data is in the wrong
     * format. Not all errors are explicitly checked for, only that
     * conversions happen correctly. Failures are either produces as
     * log messages or the action is simply silently dropped.
     *
     * The value of the edit boxes can be programmatically updated
     * through the 'val' input message port. It is also checked for
     * the correct data type.
     *
     * The \p is_pair argument to the constructor determines if the
     * edit box handles a key:value pair. If set to True, two edit
     * boxes are created with the left for the key and right for the
     * value. The key is always assumed to be a string and the value
     * is restricted by the data type setting as above.
     *
     * The block can take a default value. Because the block is
     * capable of handling multiple different data types, we enter the
     * default value as a string in the same way we expect the user to
     * enter it into the Value edit box of the widget. We convert this
     * default exactly the same way we convert the user-entered text
     * in the edit box. See the next paragraph for an explanation for
     * how to handle complex numbers.
     *
     * Complex numbers are currently handled a bit differently than
     * expected. Because we use the Boost lexical_cast function,
     * complex numbers MUST be in the form "(a,b)" to represent "a +
     * jb". Note that you cannot even have a space after the comma, so
     * "(1.23,10.56)" is correct while "(1.23, 10.56)" will not parse.
     *
     * The 'static' mode prevents the user from changing the data type
     * or the key used in the widget. If also in 'pair' mode, the key
     * is not displayed and so must be set in the constructor. It is
     * an error if using static and pair modes with no default key
     * set.
     *
     * Message Ports:
     *
     * - msg (output):
     *       Produces a PMT message from the data in the edit box. It
     *       is packaged in the PMT container determined by the \p
     *       type argument to the ctor. If the data in the box is not of
     *       the correct type and the conversion fails, the block
     *       produces a log WARN message but does nothing else with
     *       the data. If the \p is_pair flag is set on this block, it
     *       will produce a PMT pair object where the key (car) is
     *       assumed to be a string and the value (cdr) is determined
     *       by \p type.
     *
     * - val (input):
     *       Accepts messages to update the value in the edit
     *       boxes. The messages, as PMTs, are first checked to make
     *       sure that they are the correct type (integer, float,
     *       string, or complex), and unpacks them and converts them
     *       to QStrings to display in the edit box. When using \p
     *       is_pair, the PMT is checked to make sure it is a PMT
     *       pair. Then the key (car) is extracted as a string before
     *       the value (cdr) is processed based on the set data type
     *       of the box.
     */
    class QTGUI_API edit_box_msg : virtual public block
    {
    public:
      // gr::qtgui::edit_box_msg::sptr
      typedef boost::shared_ptr<edit_box_msg> sptr;

      /*!
       * \brief Constructs the Edit box block.
       *
       * \param type the data type of data in the value box.
       * \param value the default value of the message. This is
       *        entered as a string regardless of the type and
       *        converted internally -- much like how the block
       *        extracts the value from the edit box and converts it.
       * \param label a label to identify the box on screen.
       * \param is_pair if we are using a key:value pair.
       * \param is_static sets the key edit box as a static text box
       *                  (cannot be edited live).
       * \param key Set the key used in a key:value pair message.
       * \param parent a QWidget parent in the QT app.
       */
      static sptr make(gr::qtgui::data_type_t type,
                       const std::string &value="",
                       const std::string &label="",
                       bool is_pair=true,
                       bool is_static=true,
                       const std::string &key="",
                       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual QWidget* qwidget() = 0;

#ifdef ENABLE_PYTHON
      virtual PyObject* pyqwidget() = 0;
#else
      virtual void* pyqwidget() = 0;
#endif

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_EDIT_BOX_MSG_H */
