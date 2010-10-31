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

;;; The gr:connect variants
;;; These work for anything derived from gr_hier_block2
(define-method (gr:connect hb block)
  (let ((hb (coerce-to-hier-block2 hb))
	(bb (coerce-to-basic-block block)))
    (gr:connect hb bb)))

(define-method (gr:connect hb (src <gr-endpoint>) (dst <gr-endpoint>))
  (let ((hb (coerce-to-hier-block2 hb)))
    (gr:connect hb (block src) (port src) (block dst) (port dst))))

(define-method (gr:connect hb src dst)
  (let ((hb (coerce-to-hier-block2 hb))
	(src (coerce-to-endpoint src))
	(dst (coerce-to-endpoint dst)))
    (gr:connect hb src dst)))

;;; The gr:disconnect variants
;;; These work for anything derived from gr_hier_block2
(define-method (gr:disconnect-all hb)
  (let ((hb (coerce-to-hier-block2 hb)))
    (gr:disconnect-all hb)))

(define-method (gr:disconnect hb block)
  (let ((hb (coerce-to-hier-block2 hb))
	(bb (coerce-to-basic-block block)))
    (gr:disconnect hb bb)))

(define-method (gr:disconnect hb (src <gr-endpoint>) (dst <gr-endpoint>))
  (let ((hb (coerce-to-hier-block2 hb)))
    (gr:disconnect hb (block src) (port src) (block dst) (port dst))))

(define-method (gr:disconnect hb src dst)
  (let ((hb (coerce-to-hier-block2 hb))
	(src (coerce-to-endpoint src))
	(dst (coerce-to-endpoint dst)))
    (gr:disconnect hb src dst)))
