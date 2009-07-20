#!/usr/bin/guile \
-e main -s
!#
;;; -*-scheme-*-
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

(use-modules (ice-9 format))

(defmacro when (pred . body)
  `(if ,pred (begin ,@body) #f))

;; ----------------------------------------------------------------

(define (main args)

  (define (usage)
    (format 0
	    "usage: ~a <pmt-serial-tags.scm> <pmt_serial_tags.h>~%"
	    (car args)))

  (when (not (= (length args) 3))
	(usage)
	(format 0 "args: ~s~%" args)
	(exit 1))
      
  (let ((i-file (open-input-file (cadr args)))
	(h-file (open-output-file (caddr args))))

      (write-header-comment h-file "// ")
      (display "#ifndef INCLUDED_PMT_SERIAL_TAGS_H\n" h-file)
      (display "#define INCLUDED_PMT_SERIAL_TAGS_H\n" h-file)
      (newline h-file)
      (display "enum pst_tags {\n" h-file)

      (for-each-in-file i-file
       (lambda (form)
	 (let* ((name (cadr form))
		(c-name (string-upcase (c-ify name)))
		(value (caddr form)))
	   ;;(format h-file   "static const int ~a\t= 0x~x;~%" c-name value)
	   (format h-file   "  ~a\t= 0x~x,~%" c-name value))))

      (display "};\n" h-file)
      (display "#endif\n" h-file)))

(define (c-ify name)
  (list->string (map (lambda (c)
		       (if (eqv? c #\-) #\_ c))
		     (string->list (symbol->string name)))))


(define (write-header-comment o-port prefix)
  (for-each (lambda (comment)
	      (format o-port "~a~a~%" prefix comment))
	    header-comment))

(define header-comment
  '(
    ""
    "Copyright 2007 Free Software Foundation, Inc."
    ""
    "This file is part of GNU Radio"
    ""
    "GNU Radio is free software; you can redistribute it and/or modify"
    "it under the terms of the GNU General Public License as published by"
    "the Free Software Foundation; either version 3, or (at your option)"
    "any later version."
    ""
    "GNU Radio is distributed in the hope that it will be useful,"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
    "GNU General Public License for more details."
    ""
    "You should have received a copy of the GNU General Public License along"
    "with this program; if not, write to the Free Software Foundation, Inc.,"
    "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA."
    ""
    ""
    "THIS FILE IS MACHINE GENERATED FROM pmt-serial-tags.scm. DO NOT EDIT BY HAND."
    "See pmt-serial-tags.scm for additional commentary."
    ""))



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
