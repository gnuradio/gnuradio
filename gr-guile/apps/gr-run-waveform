#!/usr/bin/guile \
-e main -s
!#
;;;
;;; Copyright 2010 Free Software Foundation, Inc.
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
;;; You should have received a copy of the GNU General Public License
;;; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;;;

;;; Load and run a waveform defined with define-waveform
;;;
;;; usage: gr-run-waveform filename.wfd


(use-modules (oop goops)
	     (gnuradio core)
	     (gnuradio waveform))


(define (enable-debug-mode)
  (display %load-path)
  (newline)
  (set! %load-verbosely #t)
  (debug-enable 'backtrace 'debug))

(define (usage args)
  (let ((port (current-error-port)))
    (display "usage: " port)
    (display (car args) port)
    (newline port)
    (exit 1)))
    

(define (main args)
  (if (not (>= (length args) 2))
      (usage args))
  (enable-debug-mode)
  (let ((filename (cadr args)))
    ;; Probably ought to handle errors here 
    (load filename)
    (let ((f (waveform-last-registered)))
      (if (not f)
	  (begin
	    (format 1 "No define-waveform found in file '~A'\n" filename)
	    (exit 1)))
      (gr:run (f (cdr args))))))


;;; Local Variables:
;;; mode: scheme
;;; End:
