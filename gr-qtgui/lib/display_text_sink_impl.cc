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


#include "display_text_sink_impl.h"
#include "display_text_window.h"
#include <gnuradio/io_signature.h>


namespace gr {
namespace qtgui {

display_text_sink::sptr display_text_sink::make(const std::string& label,
                                                int splitlength,
                                                int maxlines,
                                                QWidget* parent)
{
    return gnuradio::get_initial_sptr(
        new display_text_sink_impl(label, splitlength, maxlines, parent));
}

/*
 * The private constructor
 */
display_text_sink_impl::display_text_sink_impl(const std::string& label,
                                               int splitlength,
                                               int maxlines,
                                               QWidget* parent)
    : gr::sync_block("display_text_sink",
                     gr::io_signature::make(1, 1, sizeof(char)),
                     gr::io_signature::make(0, 0, 0)),
      d_splitlength(splitlength),
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
    d_main_gui = new display_text_window(d_splitlength, maxlines, d_parent);
    d_main_gui->set_header(QString(label.c_str()));
}

/*
 * Our virtual destructor.
 */
display_text_sink_impl::~display_text_sink_impl() {}

int display_text_sink_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const char* in = (const char*)input_items[0];

    gr::thread::scoped_lock lock(d_setlock);

    // Tell runtime system how many output items we produced.
    d_main_gui->set_text(in, noutput_items);
    return noutput_items;
}

void display_text_sink_impl::exec_() { d_qApplication->exec(); }

QWidget* display_text_sink_impl::qwidget() { return d_main_gui; }

} // namespace qtgui
} /* namespace gr */
