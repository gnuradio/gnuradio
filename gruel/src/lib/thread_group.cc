/* -*- c++ -*- */
/*
 * Copyright (C) 2001-2003 William E. Kempf
 * Copyright (C) 2007 Anthony Williams
 * Copyright 2008 Free Software Foundation, Inc.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

/*
 * This was extracted from Boost 1.35.0 and fixed.
 */

#include <gruel/thread_group.h>

namespace gruel
{
  thread_group::thread_group()
  {
  }

  thread_group::~thread_group()
  {
    // We shouldn't have to scoped_lock here, since referencing this object
    // from another thread while we're deleting it in the current thread is
    // going to lead to undefined behavior any way.
    for (std::list<boost::thread*>::iterator it = m_threads.begin();
	 it != m_threads.end(); ++it)
      {
	delete (*it);
      }
  }

  boost::thread* thread_group::create_thread(const boost::function0<void>& threadfunc)
  {
    // No scoped_lock required here since the only "shared data" that's
    // modified here occurs inside add_thread which does scoped_lock.
    std::auto_ptr<boost::thread> thrd(new boost::thread(threadfunc));
    add_thread(thrd.get());
    return thrd.release();
  }

  void thread_group::add_thread(boost::thread* thrd)
  {
    boost::lock_guard<boost::shared_mutex> guard(m_mutex);

    // For now we'll simply ignore requests to add a thread object multiple
    // times. Should we consider this an error and either throw or return an
    // error value?
    std::list<boost::thread*>::iterator it = std::find(m_threads.begin(),
						       m_threads.end(), thrd);
    BOOST_ASSERT(it == m_threads.end());
    if (it == m_threads.end())
      m_threads.push_back(thrd);
  }

  void thread_group::remove_thread(boost::thread* thrd)
  {
    boost::lock_guard<boost::shared_mutex> guard(m_mutex);

    // For now we'll simply ignore requests to remove a thread object that's
    // not in the group. Should we consider this an error and either throw or
    // return an error value?
    std::list<boost::thread*>::iterator it = std::find(m_threads.begin(),
						       m_threads.end(), thrd);
    BOOST_ASSERT(it != m_threads.end());
    if (it != m_threads.end())
      m_threads.erase(it);
  }

  void thread_group::join_all()
  {
    boost::shared_lock<boost::shared_mutex> guard(m_mutex);
    for (std::list<boost::thread*>::iterator it = m_threads.begin();
	 it != m_threads.end(); ++it)
      {
	(*it)->join();
      }
  }

  void thread_group::interrupt_all()
  {
    boost::shared_lock<boost::shared_mutex> guard(m_mutex);
    for(std::list<boost::thread*>::iterator it=m_threads.begin(),end=m_threads.end();
	it!=end;
	++it)
      {
	(*it)->interrupt();
      }
  }

  size_t thread_group::size() const
  {
    boost::shared_lock<boost::shared_mutex> guard(m_mutex);
    return m_threads.size();
  }

} // namespace gruel
