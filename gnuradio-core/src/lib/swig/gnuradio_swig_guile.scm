;;;
;;; Copyright 2006,2009 Free Software Foundation, Inc.
;;; 
;;; This file is part of GNU Radio
;;; 
;;; GNU Radio is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3, or (at your option)
;;; any later version.
;;; 
;;; GNU Radio is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;; 
;;; You should have received a copy of the GNU General Public License along
;;; with this program; if not, write to the Free Software Foundation, Inc.,
;;; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;;;

;; Only use these if load-* fails t find a .scm or .so. If you can't
;; find a .so. try running ldconfig. guile seems to only load modules 
;; that are in the ldconfig paths.
;; (set! %load-path (append %load-path '("/usr/lib/guile/gnuradio")))
;; (set! %load-path (append %load-path '("/usr/share/gnuradio")))

(load "gnuradio/gnuradio_swig_py_filter.scm")
(load "gnuradio/gnuradio_swig_py_io.scm")
(load "gnuradio/gnuradio_swig_py_runtime.scm")

;; FIXME: these don't load for sme reason
;; (load "gnuradio/gnuradio_swig_py_general.scm")
;; (load "gnuradio/gnuradio_swig_py_gengen.scm")
;; (load "gnuradio/gnuradio_swig_py_heir.scm")
