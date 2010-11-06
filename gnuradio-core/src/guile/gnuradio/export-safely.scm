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

;;; This module implements a macro, export-safely, that avoids
;;; exporting symbols that are actually generic-functions imported
;;; (explicity or implicitly) from elsewhere.
;;;
;;; This hackery is required so that the swig generated goops wrappers
;;; don't stomp on each other.  For background on what this is about
;;; see this thread:
;;;
;;;   http://lists.gnu.org/archive/html/guile-user/2006-05/msg00007.html
;;;
;;; Don't expect to understand what's going on here without looking at
;;; the guts of the module system (implemented in ice-9/boot-9.scm) and
;;; having a pretty good understanding of goops and generic-functions.


(define-module (gnuradio export-safely)
  #:use-module (oop goops)
  #:use-module (srfi srfi-1)
  #:export-syntax (export-safely))

(define-public (generics-in-module module)
  (let ((lst '()))
    (module-for-each (lambda (sym var)
		       (if (variable-bound? var)
			   (let ((v (variable-ref var)))
			     (cond ((is-a? v <generic>)
				    (set! lst (cons v lst)))))))
		     module)
    lst))

(define-public (generic-function-names-in-module module)
  (map generic-function-name (generics-in-module module)))

(define-public (generic-function-names-in-imported-modules module)
  (concatenate (map generic-function-names-in-module (module-uses module))))

(define-public (export-syms-if-not-imported-gf list-of-syms)
  (let ((gf-names (generic-function-names-in-imported-modules (current-module))))
    (let ((to-export (filter (lambda (sym)
			       (not (memq sym gf-names)))
			     (delete-duplicates list-of-syms))))
      (module-export! (current-module) to-export))))
	 
(defmacro export-safely names
  `(export-syms-if-not-imported-gf ',names))


(define-public (names-in-module module)
  (let ((lst '()))
    (module-for-each (lambda (sym var)
		       (if (variable-bound? var)
			   (set! lst (cons sym lst))))
		     module)
    lst))

(define-public (names-in-imported-modules module)
  (delete-duplicates (concatenate (map names-in-module (module-uses module)))))

(define-public (re-export-all src-module-name)
  (let ((current (current-module))
	(src-module (resolve-interface src-module-name)))

    (define (ok-to-re-export? name)
      (let ((var (module-variable current name)))
	(cond ((not var) #f)					  ; Undefined var
	      ((eq? var (module-local-variable current name)) #f) ; local var
	      (else #t))))					  ; OK

    (module-re-export! current
		       (filter ok-to-re-export?
			       (names-in-module src-module)))))
