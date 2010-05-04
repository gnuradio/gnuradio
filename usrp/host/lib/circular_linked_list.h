/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
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

#ifndef _CIRCULAR_LINKED_LIST_H_
#define _CIRCULAR_LINKED_LIST_H_

#include <gruel/thread.h>
#include <stdexcept>

#define __INLINE__ inline

#ifndef DO_DEBUG
#define DO_DEBUG 0
#endif

#if DO_DEBUG
#define DEBUG(X) do{X} while(0);
#else
#define DEBUG(X) do{} while(0);
#endif

template <class T> class s_both;

template <class T> class s_node
{
  typedef s_node<T>* s_node_ptr;

private:
  T d_object;
  bool d_available;
  s_node_ptr d_prev, d_next;
  s_both<T>* d_both;

public:
  s_node (T l_object,
	  s_node_ptr l_prev = NULL,
	  s_node_ptr l_next = NULL)
    : d_object (l_object), d_available (TRUE), d_prev (l_prev),
    d_next (l_next), d_both (0) {};

  __INLINE__ s_node (s_node_ptr l_prev, s_node_ptr l_next = NULL) {
    s_node ((T) NULL, l_prev, l_next); };
  __INLINE__ s_node () { s_node (NULL, NULL, NULL); };
  __INLINE__ ~s_node () {};

  void remove () {
    d_prev->next (d_next);
    d_next->prev (d_prev);
    d_prev = d_next = this;
  };

  void insert_before (s_node_ptr l_next) {
    if (l_next) {
      s_node_ptr l_prev = l_next->prev ();
      d_next = l_next;
      d_prev = l_prev;
      l_prev->next (this);
      l_next->prev (this);
    } else
      d_next = d_prev = this;
  };

  void insert_after (s_node_ptr l_prev) {
    if (l_prev) {
      s_node_ptr l_next = l_prev->next ();
      d_prev = l_prev;
      d_next = l_next;
      l_next->prev (this);
      l_prev->next (this);
    } else
      d_prev = d_next = this;
  };

  __INLINE__ T object () { return (d_object); };
  __INLINE__ void object (T l_object) { d_object = l_object; };
  __INLINE__ bool available () { return (d_available); };
  __INLINE__ void set_available () { d_available = TRUE; };
  __INLINE__ void set_available (bool l_avail) { d_available = l_avail; };
  __INLINE__ void set_not_available () { d_available = FALSE; };
  __INLINE__ s_node_ptr next () { return (d_next); };
  __INLINE__ s_node_ptr prev () { return (d_prev); };
  __INLINE__ s_both<T>* both () { return (d_both); };
  __INLINE__ void next (s_node_ptr l_next) { d_next = l_next; };
  __INLINE__ void prev (s_node_ptr l_prev) { d_prev = l_prev; };
  __INLINE__ void both (s_both<T>* l_both) { d_both = l_both; };
};

template <class T> class circular_linked_list {
  typedef s_node<T>* s_node_ptr;

private:
  s_node_ptr d_current, d_iterate, d_available, d_inUse;
  size_t d_n_nodes, d_n_used;
  gruel::mutex* d_internal;
  gruel::condition_variable* d_ioBlock;

public:
  circular_linked_list (size_t n_nodes) {
    if (n_nodes == 0)
      throw std::runtime_error ("circular_linked_list(): n_nodes == 0");

    d_iterate = NULL;
    d_n_nodes = n_nodes;
    d_n_used = 0;
    s_node_ptr l_prev, l_next;
    d_inUse = d_current = l_next = l_prev = NULL;

    l_prev = new s_node<T> ();
    l_prev->set_available ();
    l_prev->next (l_prev);
    l_prev->prev (l_prev);
    if (n_nodes > 1) {
      l_next = new s_node<T> (l_prev, l_prev);
      l_next->set_available ();
      l_next->next (l_prev);
      l_next->prev (l_prev);
      l_prev->next (l_next);
      l_prev->prev (l_next);
      if (n_nodes > 2) {
	size_t n = n_nodes - 2;
	while (n-- > 0) {
	  d_current = new s_node<T> (l_prev, l_next);
	  d_current->set_available ();
	  d_current->prev (l_prev);
	  d_current->next (l_next);
	  l_prev->next (d_current);
	  l_next->prev (d_current);
	  l_next = d_current;
	  d_current = NULL;
	}
      }
    }
    d_available = d_current = l_prev;
    d_ioBlock = new gruel::condition_variable ();
    d_internal = new gruel::mutex ();
  };

  ~circular_linked_list () {
    iterate_start ();
    s_node_ptr l_node = iterate_next ();
    while (l_node) {
      delete l_node;
      l_node = iterate_next ();
    }
    delete d_ioBlock;
    d_ioBlock = NULL;
    delete d_internal;
    d_internal = NULL;
    d_available = d_inUse = d_iterate = d_current = NULL;
    d_n_used = d_n_nodes = 0;
  };

  s_node_ptr find_next_available_node () {
    gruel::scoped_lock l (*d_internal);
// find an available node
    s_node_ptr l_node = d_available; 
    DEBUG (std::cerr << "w ");
    while (! l_node) {
      DEBUG (std::cerr << "x" << std::endl);
      // the ioBlock condition will automatically unlock() d_internal
      d_ioBlock->wait (l);
      // and lock() is here
      DEBUG (std::cerr << "y" << std::endl);
      l_node = d_available;
    }
    DEBUG (std::cerr << "::f_n_a_n: #u = " << num_used()
	   << ", node = " << l_node << std::endl);
// remove this one from the current available list
    if (num_available () == 1) {
// last one, just set available to NULL
      d_available = NULL;
    } else
      d_available = l_node->next ();
    l_node->remove ();
// add is to the inUse list
    if (! d_inUse)
      d_inUse = l_node;
    else
      l_node->insert_before (d_inUse);
    d_n_used++;
    l_node->set_not_available ();
    return (l_node);
  };

  void make_node_available (s_node_ptr l_node) {
    if (!l_node) return;
    gruel::scoped_lock l (*d_internal);
    DEBUG (std::cerr << "::m_n_a: #u = " << num_used()
	   << ", node = " << l_node << std::endl);
// remove this node from the inUse list
    if (num_used () == 1) {
// last one, just set inUse to NULL
      d_inUse = NULL;
    } else
      d_inUse = l_node->next ();
    l_node->remove ();
// add this node to the available list
    if (! d_available)
      d_available = l_node;
    else
      l_node->insert_before (d_available);
    d_n_used--;

    DEBUG (std::cerr << "s" << d_n_used);
// signal the condition when new data arrives
    d_ioBlock->notify_one ();
    DEBUG (std::cerr << "t ");
  };

  __INLINE__ void iterate_start () { d_iterate = d_current; };

  s_node_ptr iterate_next () {
#if 0
// lock the mutex for thread safety
    gruel::scoped_lock l (*d_internal);
#endif
    s_node_ptr l_this = NULL;
    if (d_iterate) {
      l_this = d_iterate;
      d_iterate = d_iterate->next ();
      if (d_iterate == d_current)
	d_iterate = NULL;
    }
    return (l_this);
  };

  __INLINE__ T object () { return (d_current->d_object); };
  __INLINE__ void object (T l_object) { d_current->d_object = l_object; };
  __INLINE__ size_t num_nodes () { return (d_n_nodes); };
  __INLINE__ size_t num_used () { return (d_n_used); };
  __INLINE__ void num_used (size_t l_n_used) { d_n_used = l_n_used; };
  __INLINE__ size_t num_available () { return (d_n_nodes - d_n_used); };
  __INLINE__ void num_used_inc (void) {
    if (d_n_used < d_n_nodes) ++d_n_used;
  };
  __INLINE__ void num_used_dec (void) {
    if (d_n_used != 0) --d_n_used;
// signal the condition that new data has arrived
    d_ioBlock->notify_one ();
  };
  __INLINE__ bool in_use () { return (d_n_used != 0); };
};

template <class T> class s_both
{
private:
  s_node<T>* d_node;
  void* d_this;
public:
  __INLINE__ s_both (s_node<T>* l_node, void* l_this)
    : d_node (l_node), d_this (l_this) {};
  __INLINE__ ~s_both () {};
  __INLINE__ s_node<T>* node () { return (d_node); };
  __INLINE__ void* This () { return (d_this); };
  __INLINE__ void set (s_node<T>* l_node, void* l_this) {
    d_node = l_node; d_this = l_this;};
};

#endif /* _CIRCULAR_LINKED_LIST_H_ */
