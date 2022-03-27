/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DISPLAY_IMAGE_H
#define INCLUDED_DISPLAY_IMAGE_H

#ifdef ENABLE_PYTHON
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <QWidget>


class ShowPngPicture;

namespace gr {
namespace qtgui {

/*!
 * \brief display a grayscaled ( png ) image
 * \ingroup instrumentation_blk
 * \ingroup qtgui_blk
 *
 * \details
 * This block displays an grayscaled image in a
 * scrollable window
 * The image may be saved as png image to the filesystem
 *
 */
class QTGUI_API display_image : virtual public sync_block
{
public:
    typedef std::shared_ptr<display_image> sptr;

    /*!
     * \brief Build an image sink
     *
     * \param imagewidth Width of the image
     * \param imageheight Maximum height of image
     * \param parent a QWidget parent widget, may be nullptr
     *
     */
    static sptr make(int imagewidth, int imageheight, QWidget* parent = nullptr);

    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;

    virtual void displayBottomUp(bool direction) = 0;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_DISPLAY_IMAGE_H */
