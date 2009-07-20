;;; -*- scheme -*-
;;;
;;; Copyright 2007 Free Software Foundation, Inc.
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

(define-module (gnuradio macros-etc)
  :export (atom? when unless for-each-in-file))

(define (atom? obj)
  (not (pair? obj)))

(defmacro when (pred . body)
  `(if ,pred (begin ,@body) #f))

(defmacro unless (pred . body)
  `(if (not ,pred) (begin ,@body) #f))


(define (for-each-in-file file f)
  (let ((port (if (port? file)
		  file
		  (open-input-file file))))
    (letrec
     ((loop
       (lambda (port form)
	 (cond ((eof-object? form)
		(when (not (eq? port file))
		      (close-input-port port))
		#t)
	       (else
		(f form)
		(set! form #f)		; for GC
		(loop port (read port)))))))
     (loop port (read port)))))
