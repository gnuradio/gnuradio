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

(define-module (gnuradio waveform)
  #:use-module (ice-9 syncase)
  #:export-syntax (define-waveform))


(define *registry* '())			; alist
(define *last-registered* #f)


(define-syntax define-waveform
  (syntax-rules (vars blocks connections)
    ((_ (name cmd-line-args)
	(vars (v-name v-val) ...)
	(blocks (b-name b-val) ...)
	(connections (endpoint1 endpoint2 ...) ...))
     (waveform-register 'name
      (lambda (cmd-line-args)
	(let* ((v-name v-val) ...
	       (b-name b-val) ...
	       (tb (gr:top-block-swig "waveform-top-block")))
	  (gr:connect tb endpoint1 endpoint2 ...) ...
	  tb))))))


(define-public (waveform-register name thunk)
  (set! *registry* (assoc-set! *registry* name thunk))
  (set! *last-registered* thunk)
  #t)

(define-public (waveform-lookup name)
  (let ((r (assoc name *registry*)))
    (and r (cdr r))))

(define-public (waveform-last-registered)
  *last-registered*)
