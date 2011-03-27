/* -*- c++ -*- */
/*
 * Copyright 2008,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_H
#define INCLUDED_QTGUI_H

#include <gruel/thread.h>
#include <qapplication.h>
#include "SpectrumGUIClass.h"

class qtgui_event : public QEvent
{
private:
  gruel::mutex &d_mutex;

public:
  qtgui_event(gruel::mutex &mutex)
    : QEvent((QEvent::Type)(QEvent::User+101)),
    d_mutex(mutex)
  {
    //nop
  }

  ~qtgui_event() {}

  void lock()
  {
    d_mutex.lock();
  }

  void unlock()
  {
    d_mutex.unlock();
  }
};

class qtgui_obj : public QObject
{
public:
  qtgui_obj(QObject *p)
    : QObject(p)
  { 
  }

  ~qtgui_obj() {}

  void customEvent(QEvent *e)
  {
    if(e->type() == (QEvent::Type)(QEvent::User+101)) {
      qtgui_event *qt = (qtgui_event*)e;
      qt->unlock();
    }
  }
};

#endif /* INCLUDED_QTGUI_H */
