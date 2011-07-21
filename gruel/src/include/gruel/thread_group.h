/* -*- c++ -*- */
/*
 * Copyright (C) 2001-2003 William E. Kempf
 * Copyright (C) 2007 Anthony Williams
 * Copyright 2008,2009 Free Software Foundation, Inc.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

/*
 * This was extracted from Boost 1.35.0 and fixed.
 */

#ifndef INCLUDED_GRUEL_THREAD_GROUP_H
#define INCLUDED_GRUEL_THREAD_GROUP_H

#include <gruel/api.h>
#include <gruel/thread.h>
#include <boost/utility.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/function.hpp>

namespace gruel
{
  class GRUEL_API thread_group : public boost::noncopyable
  {
  public:
    thread_group();
    ~thread_group();

    boost::thread* create_thread(const boost::function0<void>& threadfunc);
    void add_thread(boost::thread* thrd);
    void remove_thread(boost::thread* thrd);
    void join_all();
    void interrupt_all();
    size_t size() const;

  private:
    std::list<boost::thread*> m_threads;
    mutable boost::shared_mutex m_mutex;
  };
}

#endif /* INCLUDED_GRUEL_THREAD_GROUP_H */
