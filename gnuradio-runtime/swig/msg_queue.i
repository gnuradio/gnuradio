/* -*- c++ -*- */
/*
 * Copyright 2005,2009-2011,2013 Free Software Foundation, Inc.
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

namespace gr {
  /*!
   * \brief thread-safe message queue
   */
  class msg_queue : public gr::msg_handler
  {
  public:
    typedef boost::shared_ptr<msg_queue> sptr;

    static sptr make(unsigned int limit=0);

    msg_queue(unsigned int limit);
    ~msg_queue();

    //! Generic msg_handler method: insert the message.
    //void handle(gr::message::sptr msg) { insert_tail (msg); }

    /*!
     * \brief Insert message at tail of queue.
     * \param msg message
     *
     * Block if queue if full.
     */
    //void insert_tail(gr::message::sptr msg);

    /*!
     * \brief Delete message from head of queue and return it.
     * Block if no message is available.
     */
    //gr::message::sptr delete_head();

    /*!
     * \brief If there's a message in the q, delete it and return it.
     * If no message is available, return 0.
     */
    gr::message::sptr delete_head_nowait();

    //! is the queue empty?
    bool empty_p() const;

    //! is the queue full?
    bool full_p() const;

    //! return number of messages in queue
    unsigned int count() const;

    //! Delete all messages from the queue
    void flush();
  };
}

/*
 * The following kludge-o-rama releases the Python global interpreter
 * lock around these potentially blocking calls.  We don't want
 * libgnuradio-runtime to be dependent on Python, thus we create these
 * functions that serve as replacements for the normal C++ delete_head
 * and insert_tail methods.  The %pythoncode smashes these new C++
 * functions into the gr.msg_queue wrapper class, so that everything
 * appears normal.  (An evil laugh is heard in the distance...)
 */
#ifdef SWIGPYTHON
%inline %{
  gr::message::sptr py_msg_queue__delete_head(gr::msg_queue::sptr q) {
    gr::message::sptr msg;
    GR_PYTHON_BLOCKING_CODE(
        msg = q->delete_head();
    )
    return msg;
  }

  void py_msg_queue__insert_tail(gr::msg_queue::sptr q, gr::message::sptr msg) {
    GR_PYTHON_BLOCKING_CODE(
        q->insert_tail(msg);
    )
  }
%}

// smash in new python delete_head and insert_tail methods...
%template(msg_queue_sptr) boost::shared_ptr<gr::msg_queue>;
%pythoncode %{
msg_queue_sptr.delete_head = py_msg_queue__delete_head
msg_queue_sptr.insert_tail = py_msg_queue__insert_tail
msg_queue_sptr.handle = py_msg_queue__insert_tail
msg_queue = msg_queue.make
%}
#endif	// SWIGPYTHON
