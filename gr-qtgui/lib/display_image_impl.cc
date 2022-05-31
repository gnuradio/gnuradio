/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "display_image_impl.h"
#include "showpngpicture.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace qtgui {

display_image::sptr display_image::make(int imagewidth, int imageheight, QWidget* parent)
{
    return gnuradio::make_block_sptr<display_image_impl>(imagewidth, imageheight, parent);
}

/*
 * The private constructor
 */
display_image_impl::display_image_impl(int imagewidth, int imageheight, QWidget* parent)
    : gr::sync_block("display_image",
                     gr::io_signature::make(2, 2, sizeof(char)),
                     gr::io_signature::make(0, 0, 0)),
      d_width(imagewidth),
      d_height(imageheight),
      d_parent(parent)
{
    d_main_gui = NULL;
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        int argc = 1;
        char* argv = new char;
        argv[0] = '\0';
        d_qApplication = new QApplication(argc, &argv);
    }
    d_main_gui = new ShowPngPicture(d_width, d_height, d_parent);
    d_triggered = false;
}

/*
 * Our virtual destructor.
 */
display_image_impl::~display_image_impl() {}

int display_image_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    const char* ctrl = (const char*)input_items[1];

    gr::thread::scoped_lock lock(d_setlock);

    if (d_triggered) {
        // Do signal processing
        d_main_gui->setPixel(in, noutput_items);
    } else {
        int i = 0;
        while (i < noutput_items && ctrl[i] == 0) {
            i++;
        }
        if (i < noutput_items) {
            d_triggered = true;
            d_main_gui->setPixel(in + i, noutput_items - i);
        }
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

void display_image_impl::displayBottomUp(bool direction)
{
    d_main_gui->presetBottomUp(direction);
}

QWidget* display_image_impl::qwidget() { return d_main_gui; }

void display_image_impl::exec_() { d_qApplication->exec(); }

} // namespace qtgui
} /* namespace gr */
