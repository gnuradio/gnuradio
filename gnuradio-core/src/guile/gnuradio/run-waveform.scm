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


;; I don't seem to be able to make this work...
;; I think it's some kind of interaction with the syntax-case
;; macro, define-waveform, and the module system.
;;
;;(define-module (gnuradio run-waveform)
;;  #:use-module (oop goops)
;;  #:use-module (gnuradio core)
;;  #:use-module (gnuradio waveform)
;;  #:duplicates (merge-generics replace check))

(use-modules (oop goops)
	     (gnuradio core)
	     (gnuradio waveform))


(define (load-into-module filename module)
  (let ((f (open-file filename "r")))
    (let loop ((form (read f)))
      (cond ((eof-object? form) #t)
	    (else (eval form module)
		  (loop (read f)))))))


(define-public (run-waveform waveform-filename . args)
  (debug-enable 'backtrace 'debug)
  (load waveform-filename)
  ;;(load-into-module waveform-filename (current-module))
  (let ((f (waveform-last-registered)))
    (if (not f)
	(error "No define-waveform found in file \n" filename))
    (gr:run (f args))
    ;; Attempt to get block destructors called now.
    (gc)))
