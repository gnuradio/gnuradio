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

(define-module (gnuradio runtime-shim)
  #:use-module (oop goops)
  #:use-module (ice-9 threads)
  #:use-module (gnuradio gnuradio_core_runtime)
  #:duplicates (merge-generics replace check))

(define-class <gr-endpoint> (<object>)
  (block #:accessor block #:init-keyword #:block)
  (port #:init-value 0 #:accessor port #:init-keyword #:port))

(define (gr:ep block port)
  (make <gr-endpoint>
    #:block (coerce-to-basic-block block) #:port port))

(define (coerce-to-endpoint ep)
  (cond ((is-a? ep <gr-endpoint>) ep)
	((false-if-exception (gr:to-basic-block ep))
	 => (lambda (x) (gr:ep x 0)))
	((and (pair? ep) (= 2 (length ep))
	      (false-if-exception (gr:to-basic-block (car ep))))
	 => (lambda (x) (gr:ep x (cadr ep))))
	(else (error "Cannot coerce to an endpoint: " ep))))

(define (coerce-to-basic-block block)
  (cond ((is-a? block <gr-basic-block-sptr>) block)
	((false-if-exception (gr:to-basic-block block)) => (lambda (x) x))
	(else (error "Cannot coerce to a gr_basic_block: " block))))

(define (coerce-to-top-block block)
  (cond ((is-a? block <gr-top-block-sptr>) block)
	((false-if-exception (gr:to-top-block block)) => (lambda (x) x))
	(else (error "Cannot coerce to a gr_top_block: " block))))

(define (coerce-to-hier-block2 block)
  (cond ((is-a? block <gr-hier-block2-sptr>) block)
	((false-if-exception (gr:to-hier-block2 block)) => (lambda (x) x))
	(else (error "Cannot coerce to a gr_hier_block2: " block))))


;;; Connect one or more block endpoints.  An endpoint is either a <gr-endpoint>,
;;; a 2-list (block port), or a block instance.  In the latter case, the port number
;;; is assumed to be zero.
;;;
;;; If multiple arguments are provided, connect will attempt to wire them in series,
;;; interpreting the endpoints as inputs or outputs as appropriate.
(define-method (gr:connect hb . points)
  (dis/connect "connect" gr:primitive-connect hb points))

;;; Disconnect one or more block endpoints...
(define-method (gr:disconnect hb . points)
  (dis/connect "disconnect" gr:primitive-disconnect hb points))

(define (dis/connect name gf hb points)
  (let ((hb (coerce-to-hier-block2 hb))
	(points (list->vector (map coerce-to-endpoint points))))

    (define (op2 p0 p1)
      (gf hb (block p0) (port p0) (block p1) (port p1)))

    (let ((len (vector-length points)))
      (case len
	((0) (error (string-append name " requires at least 1 endpoint;  None provided.")))
	((1) (gf hb (vector-ref points 0)))
	(else
	 (let loop ((n 1))
	   (cond ((< n len)
		  (op2 (vector-ref points (1- n)) (vector-ref points n))
		  (loop (1+ n))))))))))




(define-method (gr:run (self <gr-top-block-sptr>))
  (gr:start self)
  (gr:wait self))


(define-method (gr:wait (tb <gr-top-block-sptr>))

  (define (sigint-handler sig)
    ;;(display "\nSIGINT!\n" (current-error-port))
    ;; tell flow graph to stop
    (gr:stop tb))

  (let ((old-handler #f))
    (dynamic-wind

	;; Called at entry
	(lambda ()
	  ;; Install SIGINT handler
	  (set! old-handler (sigaction SIGINT sigint-handler)))

	;; Protected thunk
	(lambda ()
	  (let ((waiter (begin-thread (gr:top-block-wait-unlocked tb))))
	    (join-thread waiter)
	    ;;(display "\nAfter join-thread\n" (current-error-port))
	    ))

	;; Called at exit
	(lambda ()
	  ;; Restore SIGINT handler
	  (if (not (car old-handler))
	      ;; restore original C handler
	      (sigaction SIGINT #f)
	      ;; restore Scheme handler, SIG_IGN or SIG_DFL
	      (sigaction SIGINT (car old-handler) (cdr old-handler)))))))


(export-safely <gr-endpoint> gr:ep gr:connect gr:disconnect gr:run gr:wait)
