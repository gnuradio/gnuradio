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

(define-module (gnuradio waveform-spec)
  #:use-module (ice-9 syncase)
  #:export-syntax waveform-spec)

(define-syntax waveform-spec
  (syntax-rules (vars blocks connections)
    ((_ (args ...)
	(vars (v-name v-val) ...)
	(blocks (b-name b-val) ...)
	(connections (endpoint1 endpoint2 ...) ...))
     (lambda (args ...)
       (let* ((v-name v-val) ...
	      (b-name b-val) ...
	      (tb (gr:top-block-swig "waveform-top-block")))
	 (gr:connect tb endpoint1 endpoint2 ...) ...
	 tb)))))

