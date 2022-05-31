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

#include "display_text_msg_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>


namespace gr {
namespace qtgui {

display_text_msg::sptr display_text_msg::make(const std::string& label,
                                              const std::string& message_key,
                                              int splitlength,
                                              int maxline,
                                              QWidget* parent)
{
    return gnuradio::make_block_sptr<display_text_msg_impl>(
        label, message_key, splitlength, maxline, parent);
}

display_text_msg_impl::display_text_msg_impl(const std::string& label,
                                             const std::string& message_key,
                                             int splitlength,
                                             int maxlines,
                                             QWidget* parent)
    : block("display_text_msg", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0))
{
    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    d_argc = 1;
    d_argv = new char;
    d_argv[0] = '\0';

    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    d_splitlength = splitlength;
    d_text = new display_text_window(d_splitlength, maxlines, parent);
    d_text->set_header(QString(label.c_str()));

    if (message_key.empty())
        d_message_key = pmt::PMT_NIL;
    else
        d_message_key = pmt::string_to_symbol(message_key);
    message_port_register_in(pmt::mp("text"));
    set_msg_handler(pmt::mp("text"), [this](pmt::pmt_t msg) {
        this->display_text_msg_impl::set_value(msg);
    });
}

display_text_msg_impl::~display_text_msg_impl() { delete d_argv; }


void display_text_msg_impl::exec_() { d_qApplication->exec(); }

QWidget* display_text_msg_impl::qwidget() { return (QWidget*)d_text; }

void display_text_msg_impl::set_value(pmt::pmt_t val)
{

    std::string xs;

    if (pmt::is_pair(val)) { // Check, if we received a pair
        pmt::pmt_t key = pmt::car(val);
        if (pmt::eq(key, d_message_key)) {
            pmt::pmt_t msg = pmt::cdr(val);
            if (pmt::is_symbol(msg)) {
                xs = pmt::symbol_to_string(msg);
            } else {
                if (pmt::is_u8vector(msg)) {
                    size_t len;
                    const uint8_t* c = pmt::u8vector_elements(msg, len);
                    d_logger->warn("Found a vector of length = {:d} ", len);
                    for (size_t i = 0; i < len; i++) {
                        xs += c[i];
                        if (c[i] == '\n')
                            d_logger->warn("Newline found at {:d}", i);
                    }
                    xs += '\n';
                } else {
                    d_logger->warn(
                        "Message pair did not contain a valid text message or vector");
                    return;
                }
            }
        } else {
            d_logger->warn("Message must have the key = {:s} ; got {:s}.",
                           pmt::write_string(d_message_key),
                           pmt::write_string(key));
            return;
        }
    } else {
        if (pmt::is_symbol(val)) {
            xs = pmt::symbol_to_string(val);
        } else {
            d_logger->warn("Did not find a valid message");
            return;
        }
    }

    d_text->set_text(xs.c_str(), xs.size());
}


} /* namespace qtgui */
} /* namespace gr */
