/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

class gr_msg_queue;
typedef boost::shared_ptr<gr_msg_queue> gr_msg_queue_sptr;
%template(gr_msg_queue_sptr) boost::shared_ptr<gr_msg_queue>;

%rename(msg_queue) gr_make_msg_queue;
gr_msg_queue_sptr gr_make_msg_queue(unsigned limit=0);

/*!
 * \brief thread-safe message queue
 */
%ignore gr_msg_queue;
class gr_msg_queue : public gr_msg_handler {
  omni_mutex		d_mutex;
  omni_condition	d_cond;
  gr_message_sptr	d_head;
  gr_message_sptr	d_tail;
  int			d_count;

public:
  gr_msg_queue();
  ~gr_msg_queue();

  //! Generic msg_handler method: insert the message.
  //void handle(gr_message_sptr msg) { insert_tail (msg); }

  /*!
   * \brief Insert message at tail of queue.
   * \param msg message
   *
   * Block if queue if full.
   */
  //void insert_tail(gr_message_sptr msg);

  /*!
   * \brief Delete message from head of queue and return it.
   * Block if no message is available.
   */
  //gr_message_sptr delete_head();

  /*!
   * \brief If there's a message in the q, delete it and return it.
   * If no message is available, return 0.
   */
  gr_message_sptr delete_head_nowait();
  
  //! is the queue empty?
  bool empty_p() const;
  
  //! is the queue full?
  bool full_p() const;
  
  //! return number of messages in queue
  unsigned int count() const;

  //! Delete all messages from the queue
  void flush();
};

/*
 * The following kludge-o-rama releases the Python global interpreter
 * lock around these potentially blocking calls.  We don't want
 * libgnuradio-core to be dependent on Python, thus we create these
 * functions that serve as replacements for the normal C++ delete_head
 * and insert_tail methods.  The %pythoncode smashes these new C++
 * functions into the gr.msg_queue wrapper class, so that everything
 * appears normal.  (An evil laugh is heard in the distance...)
 */
%inline {
  gr_message_sptr gr_py_msg_queue__delete_head(gr_msg_queue_sptr q) {
    gr_message_sptr msg;
    Py_BEGIN_ALLOW_THREADS;		// release global interpreter lock
    msg = q->delete_head();		// possibly blocking call
    Py_END_ALLOW_THREADS;		// acquire global interpreter lock
    return msg;
  }

  void gr_py_msg_queue__insert_tail(gr_msg_queue_sptr q, gr_message_sptr msg) {
    Py_BEGIN_ALLOW_THREADS;		// release global interpreter lock
    q->insert_tail(msg);		// possibly blocking call
    Py_END_ALLOW_THREADS;		// acquire global interpreter lock
  }
}

// smash in new python delete_head and insert_tail methods...
%pythoncode %{
gr_msg_queue_sptr.delete_head = gr_py_msg_queue__delete_head
gr_msg_queue_sptr.insert_tail = gr_py_msg_queue__insert_tail
gr_msg_queue_sptr.handle = gr_py_msg_queue__insert_tail
%}
