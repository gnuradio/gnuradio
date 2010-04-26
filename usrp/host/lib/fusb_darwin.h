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

#ifndef _FUSB_DARWIN_H_
#define _FUSB_DARWIN_H_

#include <usb.h>
#include "fusb.h"
#include <IOKit/IOCFBundle.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOKitLib.h>
#include "circular_linked_list.h"
#include "circular_buffer.h"

// for MacOS X 10.4.[0-3]
#define usb_interface_t IOUSBInterfaceInterface220
#define InterfaceInterfaceID kIOUSBInterfaceInterfaceID220
#define InterfaceVersion 220

// for MacOS X 10.3.[0-9] and 10.4.[0-3]
#define usb_device_t    IOUSBDeviceInterface197
#define DeviceInterfaceID kIOUSBDeviceInterfaceID197
#define DeviceVersion 197

extern "C" {
typedef struct usb_dev_handle {
  int fd;

  struct usb_bus *bus;
  struct usb_device *device;

  int config;
  int interface;
  int altsetting;

  /* Added by RMT so implementations can store other per-open-device data */
  void *impl_info;
} usb_dev_handle;

/* Darwin/OS X impl does not use fd field, instead it uses this */
typedef struct darwin_dev_handle {
  usb_device_t** device;
  usb_interface_t** interface;
  int open;
} darwin_dev_handle;

typedef IOReturn io_return_t;
typedef IOCFPlugInInterface *io_cf_plugin_ref_t;

static int ep_to_pipeRef (darwin_dev_handle* device, int ep);
extern int usb_debug;
}

class s_buffer
{
private:
  char* d_buffer;
  size_t d_n_used, d_n_alloc;

public:
  inline s_buffer (size_t n_alloc = 0) {
    d_n_used = 0;
    d_n_alloc = n_alloc;
    if (n_alloc) {
      d_buffer = (char*) new char [n_alloc];
    } else {
      d_buffer = 0;
    }
  };
  inline ~s_buffer () {
    if (d_n_alloc) {
      delete [] d_buffer;
    }
  };
  inline size_t n_used () { return (d_n_used); };
  inline void n_used (size_t bufLen) {
    d_n_used = (bufLen > d_n_alloc) ? d_n_alloc : bufLen; };
  inline size_t n_alloc () { return (d_n_alloc); };
  void buffer (char* l_buffer, size_t bufLen) {
    if (bufLen > d_n_alloc) {
      std::cerr << "s_buffer::set: Copying only allocated bytes." << std::endl;
      bufLen = d_n_alloc;
    }
    if (!l_buffer) {
      std::cerr << "s_buffer::set: NULL buffer." << std::endl;
      return;
    }
    bcopy (l_buffer, d_buffer, bufLen);
    d_n_used = bufLen;
  };
  inline char* buffer () { return (d_buffer); };
  inline void reset () {
    bzero (d_buffer, d_n_alloc);
    d_n_used = 0;
  };
};

typedef s_buffer* s_buffer_ptr;
typedef s_node<s_buffer_ptr>* s_node_ptr;
typedef circular_linked_list<s_buffer_ptr>* s_queue_ptr;
typedef s_both<s_buffer_ptr>* s_both_ptr;

/*!
 * \brief darwin implementation of fusb_devhandle
 *
 * This is currently identical to the generic implementation
 * and is intended as a starting point for whatever magic is
 * required to make usb fly.
 */
class fusb_devhandle_darwin : public fusb_devhandle
{
public:
  // CREATORS
  fusb_devhandle_darwin (usb_dev_handle* udh);
  virtual ~fusb_devhandle_darwin ();

  // MANIPULATORS
  virtual fusb_ephandle* make_ephandle (int endpoint, bool input_p,
					int block_size = 0, int nblocks = 0);
};

/*!
 * \brief darwin implementation of fusb_ephandle
 *
 * This is currently identical to the generic implementation
 * and is intended as a starting point for whatever magic is
 * required to make usb fly.
 */
class fusb_ephandle_darwin : public fusb_ephandle
{
private:
  fusb_devhandle_darwin* d_devhandle;
  gruel::thread* d_runThread;
  gruel::mutex* d_runThreadRunning;

  CFRunLoopRef d_CFRunLoopRef;

  static void write_completed (void* ret_io_size,
			       io_return_t result,
			       void* io_size);
  static void read_completed (void* ret_io_size,
			      io_return_t result,
			      void* io_size);
  static void run_thread (void* arg);
  static void read_thread (void* arg);

  void read_issue (s_both_ptr l_both);

public:
  // variables, for now
  UInt8 d_pipeRef, d_transferType;
  usb_interface_t** d_interfaceRef;
  usb_interface_t* d_interface;
  s_queue_ptr d_queue;
  circular_buffer<char>* d_buffer;
  size_t d_bufLenBytes;
  gruel::mutex* d_readRunning;
  gruel::mutex* d_runBlock_mutex;
  gruel::mutex* d_readBlock_mutex;
  gruel::condition_variable* d_runBlock;
  gruel::condition_variable* d_readBlock;

// CREATORS

  fusb_ephandle_darwin (fusb_devhandle_darwin *dh, int endpoint, bool input_p,
			 int block_size = 0, int nblocks = 0);
  virtual ~fusb_ephandle_darwin ();

// MANIPULATORS

  virtual bool start ();  	//!< begin streaming i/o
  virtual bool stop ();		//!< stop streaming i/o

  /*!
   * \returns \p nbytes if write was successfully enqueued, else -1.
   * Will block if no free buffers available.
   */
  virtual int write (const void* buffer, int nbytes);

  /*!
   * \returns number of bytes read or -1 if error.
   * number of bytes read will be <= nbytes.
   * Will block if no input available.
   */
  virtual int read (void* buffer, int nbytes);

  /*
   * abort any pending IO transfers
   */
  void abort ();

  /*
   * block until all outstanding writes have completed
   */
  virtual void wait_for_completion ();
};

#endif /* _FUSB_DARWIN_H_ */
