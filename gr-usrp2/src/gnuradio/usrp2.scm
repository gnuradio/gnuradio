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

;;; Module that re-exports the usrp2_swig module and adds convenience functions

(define-module (gnuradio usrp2)
  #:use-module (oop goops)
  #:use-module (gnuradio export-safely)
  #:use-module (gnuradio usrp2_swig)
  #:duplicates (merge-generics replace check))

(re-export-all '(gnuradio usrp2_swig))

;; Utilities (guaranteed not to leak the allocated object)
(define (call-with-long-ptr fn)
  (let ((ptr #f))
    (dynamic-wind
	(lambda () (set! ptr (gr:make-long-ptr)))
	(lambda () (and (fn ptr) (gr:deref-long-ptr ptr)))
	(lambda () (gr:free-long-ptr ptr)))))

(define (call-with-int-ptr fn)
  (let ((ptr #f))
    (dynamic-wind
	(lambda () (set! ptr (gr:make-int-ptr)))
	(lambda () (and (fn ptr) (gr:deref-int-ptr ptr)))
	(lambda () (gr:free-int-ptr ptr)))))

(define (call-with-uint16-ptr fn)
  (let ((ptr #f))
    (dynamic-wind
	(lambda () (set! ptr (gr:make-uint16-ptr)))
	(lambda () (and (fn ptr) (gr:deref-uint16-ptr ptr)))
	(lambda () (gr:free-uint16-ptr ptr)))))


;;; scheme-ish convenience functions

(define-method (gr:set-center-freq self freq)
  (let ((tr (make <tune-result>)))
    (if (gr:-real-set-center-freq self freq tr)
	tr
	#f)))

(define-method (gr:fpga-master-clock-freq self)
  (call-with-long-ptr (lambda (ptr) (gr:-real-fpga-master-clock-freq self ptr))))

(define-method (gr:adc-rate self)
  (call-with-long-ptr (lambda (ptr) (gr:-real-adc-rate self ptr))))

(define-method (gr:dac-rate self)
  (call-with-long-ptr (lambda (ptr) (gr:-real-dac-rate self ptr))))

(define-method (gr:daughterboard-id self)
  (call-with-int-ptr (lambda (ptr) (gr:-real-daugherboard-id self ptr))))

;; (define-method (gr:default-tx-scale-iq self) ...)  FIXME

(define-method (gr:read-gpio self)
  (call-with-uint16-ptr (lambda (ptr) (gr:-real-read-gpio self ptr))))

;; Export them
(export-safely gr:set-center-freq gr:fpga-master-clock-freq gr:adc-rate gr:dac-rate gr:daughterboard-id gr:read-gpio)
