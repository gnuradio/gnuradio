/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_NUMBER_SINK_H
#define INCLUDED_QTGUI_NUMBER_SINK_H

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/qtgui_types.h>
#include <gnuradio/qtgui/trigger_mode.h>
#include <gnuradio/sync_block.h>
#include <QApplication>

namespace gr {
namespace qtgui {

/*!
 * \brief A graphical sink to display numerical values of input streams.
 * \ingroup instrumentation_blk
 * \ingroup qtgui_blk
 *
 * \details
 *
 * Displays the data stream in as a number in a simple text box
 * GUI along with an optional bar graph. The bar graph can be set
 * to horizontal (NUM_GRAPH_HORIZ), vertical (NUM_GRAPH_VERT), or
 * no graph (NUM_GRAPH_NONE).
 *
 * The displayed value can be the average of the input stream, in
 * which case all items received are averaged. If not averaging,
 * the display simply samples a value in the data stream based on
 * the update time of this block.
 *
 * The block reads 32-bit floats and 32-, 16- and 8-bit signed
 * integers. The type of the input streams is selected with the
 * item_type_t argument of the constructor. Items are converted to
 * float before they are displayed, so integers that need more than 24
 * bits of mantissa are rounded.
 */
class QTGUI_API number_sink : virtual public sync_block
{
public:
    // gr::qtgui::number_sink::sptr
    typedef std::shared_ptr<number_sink> sptr;

    /*!
     * \brief Type of the items on the input streams
     */
    enum class item_type_t {
        FLOAT32, //!< 32 bit floating point
        INT32,   //!< 32 bit signed integer
        INT16,   //!< 16 bit signed integer
        INT8,    //!< 8 bit signed integer
    };

    /*!
     * \brief Build a number sink
     *
     * \param itemtype Type of the items on the input streams
     * \param average Averaging coefficient (0 - 1)
     * \param graph_type Type of graph to use (number_sink::graph_t)
     * \param nconnections number of signals connected to sink
     * \param parent a QWidget parent object, if any
     */
    static sptr make(item_type_t itemtype,
                     float average = 0,
                     graph_t graph_type = NUM_GRAPH_HORIZ,
                     int nconnections = 1,
                     QWidget* parent = NULL);

    /*!
     * \brief Build a number sink from an item size
     *
     * Kept for backwards compatibility. One byte selects INT8, two bytes
     * INT16 and four bytes FLOAT32; any other size throws
     * std::runtime_error. Four bytes cannot mean INT32 here, so int32
     * inputs need the item_type_t overload.
     *
     * \param itemsize Size of input item stream
     * \param average Averaging coefficient (0 - 1)
     * \param graph_type Type of graph to use (number_sink::graph_t)
     * \param nconnections number of signals connected to sink
     * \param parent a QWidget parent object, if any
     */
    static sptr make(size_t itemsize,
                     float average = 0,
                     graph_t graph_type = NUM_GRAPH_HORIZ,
                     int nconnections = 1,
                     QWidget* parent = NULL);

    virtual void exec() = 0;
    virtual QWidget* qwidget() = 0;

    virtual void set_update_time(double t) = 0;
    virtual void set_average(const float avg) = 0;
    virtual void set_graph_type(const graph_t type) = 0;
    virtual void
    set_color(unsigned int which, const std::string& min, const std::string& max) = 0;
    virtual void set_color(unsigned int which, int min, int max) = 0;
    virtual void set_label(unsigned int which, const std::string& label) = 0;
    virtual void set_min(unsigned int which, float min) = 0;
    virtual void set_max(unsigned int which, float max) = 0;
    virtual void set_title(const std::string& title) = 0;
    virtual void set_unit(unsigned int which, const std::string& unit) = 0;
    virtual void set_factor(unsigned int which, float factor) = 0;

    virtual float average() const = 0;
    virtual graph_t graph_type() const = 0;
    virtual std::string color_min(unsigned int which) const = 0;
    virtual std::string color_max(unsigned int which) const = 0;
    virtual std::string label(unsigned int which) const = 0;
    virtual float min(unsigned int which) const = 0;
    virtual float max(unsigned int which) const = 0;
    virtual std::string title() const = 0;
    virtual std::string unit(unsigned int which) const = 0;
    virtual float factor(unsigned int which) const = 0;

    virtual void enable_menu(bool en = true) = 0;
    virtual void enable_autoscale(bool en = true) = 0;

    virtual void reset() = 0;

    QApplication* d_qApplication;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_NUMBER_SINK_H */
