/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_DISPLAY_TEXT_SINK_H
#define INCLUDED_QTGUI_DISPLAY_TEXT_SINK_H

#ifdef ENABLE_PYTHON
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>

#include <QWidget>

#include <string>

namespace gr {
namespace qtgui {

/*!
 * \brief Create a QT Text sink, where the values are received as stream.
 * \ingroup instrumentation_blk
 * \ingroup qtgui_blk
 *
 * \details
 * This is a QT-based graphical sink that display simply ascii text in  a scrollable
 * window
 */
class QTGUI_API display_text_sink : virtual public sync_block
{
public:
    typedef std::shared_ptr<display_text_sink> sptr;

    /*!
     * \brief Build a graphical sink to display ascii text
     *
     * \param label Header text of the window
     *              usefull  if using several windows
     * \param splitlength enter newline after splitlength
     *                    characters without newline
     * \param maxlines maximum number of lines that
     *                  can be displayed in the scrollarea
     * \param parent a QWidget parent in the QT app.
     */
    static sptr make(const std::string& label,
                     int splitlength = 80,
                     int maxlines = 100,
                     QWidget* parent = nullptr);
    virtual QWidget* qwidget() = 0;
    virtual void exec_() = 0;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_DISPLAY_TEXT_SINK_H*/
