/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <qapplication.h>
#include "SpectrumGUIClass.h"

class qtgui_event : public QEvent
{
private:
  pthread_mutex_t *pmutex;

public:
  qtgui_event(pthread_mutex_t *mut)
    : QEvent((QEvent::Type)(QEvent::User+101))
  {
    pmutex = mut;
  }

  void lock()
  {
    pthread_mutex_lock(pmutex);
    
  }

  void unlock()
  {
    pthread_mutex_unlock(pmutex);
  }
};

class qtgui_obj : public QObject
{
public:
  qtgui_obj(QObject *p)
    : QObject(p)
  { 
  }
  
  void customEvent(QEvent *e)
  {
    if(e->type() == (QEvent::Type)(QEvent::User+101)) {
      qtgui_event *qt = (qtgui_event*)e;
      qt->unlock();
    }
  }
};

#endif /* INCLUDED_QTGUI_H */
