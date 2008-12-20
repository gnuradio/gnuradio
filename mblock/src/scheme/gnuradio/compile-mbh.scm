#!/usr/bin/guile \
-e main -s
!#
;; -*-scheme-*-
;;
;; Copyright 2007,2008 Free Software Foundation, Inc.
;; 
;; This file is part of GNU Radio
;; 
;; GNU Radio is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;; 
;; GNU Radio is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License along
;; with this program; if not, write to the Free Software Foundation, Inc.,
;; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;;

;; usage: compile-mbh <input-file> <output-file>

(use-modules (ice-9 getopt-long))
(use-modules (ice-9 format))
(use-modules (ice-9 pretty-print))
;(use-modules (ice-9 slib))
(use-modules (gnuradio pmt-serialize))
(use-modules (gnuradio macros-etc))

(debug-enable 'backtrace)

;; ----------------------------------------------------------------

(define (main args)

  (define (usage)
    (format 0 "usage: ~a input-file output-file~%" (car args)))

  (when (not (= (length args) 3))
	(usage)
	(exit 1))
      
  (let ((input-filename (cadr args))
	(output-filename (caddr args)))
      (if (compile-mbh-file input-filename output-filename)
	  (exit 0)
	  (exit 1))))


;; ----------------------------------------------------------------
;; constructor and accessors for protocol-class

(define %protocol-class-tag (string->symbol "[PROTOCOL-CLASS-TAG]"))

(define (make-protocol-class name incoming outgoing)
  (vector %protocol-class-tag name incoming outgoing))

(define (protocol-class? obj)
  (and (vector? obj) (eq? %protocol-class-tag (vector-ref obj 0))))

(define (protocol-class-name pc)
  (vector-ref pc 1))

(define (protocol-class-incoming pc)
  (vector-ref pc 2))

(define (protocol-class-outgoing pc)
  (vector-ref pc 3))


;; ----------------------------------------------------------------

(define (syntax-error msg e)
  (throw 'syntax-error msg e))

(define (unrecognized-form form)
  (syntax-error "Unrecognized form" form))


(define (mbh-chk-length= e y n)
  (cond ((and (null? y)(zero? n))
	 #f)
        ((null? y)
         (syntax-error "Expression has too few subexpressions" e))
        ((atom? y)
         (syntax-error (if (atom? e)
                           "List expected"
                           "Expression ends with `dotted' atom")
                       e))
        ((zero? n)
         (syntax-error "Expression has too many subexpressions" e))
        (else
          (mbh-chk-length= e (cdr y) (- n 1)))))

(define (mbh-chk-length>= e y n)
  (cond ((and (null? y)(< n 1))
	 #f)
        ((atom? y)
         (mbh-chk-length= e y -1))
        (else
          (mbh-chk-length>= e (cdr y) (- n 1)))))


(define (compile-mbh-file input-filename output-filename)
  (let ((i-port (open-input-file input-filename))
	(o-port (open-output-file output-filename)))

    (letrec
      ((protocol-classes '())		; alist

       (lookup-protocol-class		; returns protocol-class or #f
	(lambda (name)
	  (cond ((assq name protocol-classes) => cdr)
		(else #f))))

       (register-protocol-class
	(lambda (pc)
	  (set! protocol-classes (acons (protocol-class-name pc)
					  pc protocol-classes))
	  pc))
					  
       (parse-top-level-form
	(lambda (form)
	  (mbh-chk-length>= form form 1)
	  (case (car form)
	    ((define-protocol-class) (parse-define-protocol-class form))
	    (else (syntax-error form)))))

       (parse-define-protocol-class
	(lambda (form)               
	  (mbh-chk-length>= form form 2)
	    ;; form => (define-protocol-class name
	    ;;           (:include protocol-class-name)
	    ;;           (:incoming list-of-msgs)
	    ;;           (:outgoing list-of-msgs))
	    (let ((name (cadr form))
		  (incoming '())
		  (outgoing '()))
	      (if (lookup-protocol-class name)
		  (syntax-error "Duplicate protocol-class name" name))
	      (for-each
	       (lambda (sub-form)
		 (mbh-chk-length>= sub-form sub-form 1)
		 (case (car sub-form)
		   ((:include)
		    (mbh-chk-length>= sub-form sub-form 2)
		    (cond ((lookup-protocol-class (cadr sub-form)) =>
			   (lambda (pc)
			     (set! incoming (append incoming (protocol-class-incoming pc)))
			     (set! outgoing (append outgoing (protocol-class-outgoing pc)))))
			  (else
			   (syntax-error "Unknown protocol-class-name" (cadr sub-form)))))
		   ((:incoming)
		    (set! incoming (append incoming (cdr sub-form))))
		   ((:outgoing)
		    (set! outgoing (append outgoing (cdr sub-form))))
		   (else
		    (unrecognized-form (car sub-form)))))
	       (cddr form))
	      
	      (register-protocol-class (make-protocol-class name incoming outgoing)))))

       ) ; end of bindings

      (for-each-in-file i-port parse-top-level-form)

      ;; generate the output here...

      (letrec ((classes (map cdr protocol-classes))
	       (so-stream (make-serial-output-stream))
	       (format-output-for-c++
		(lambda (output)
		  (format o-port "//~%")
		  (format o-port "// Machine generated by compile-mbh from ~a~%" input-filename)
		  (format o-port "//~%")
		  (format o-port "// protocol-classes: ~{~a ~}~%" (map car protocol-classes))
		  (format o-port "//~%")

		  (format o-port "#include <mblock/protocol_class.h>~%")
		  (format o-port "#include <unistd.h>~%")
		  (format o-port
			  "static const char~%protocol_class_init_data[~d] = {~%  "
			  (length output))

		  (do ((lst output (cdr lst))
		       (i 0 (+ i 1)))
		      ((null? lst) #t)
		    (format o-port "~a, " (car lst))
		    (when (= 15 (modulo i 16))
			  (format o-port "~%  ")))

		  (format o-port "~&};~%")
		  (format o-port "static mb_protocol_class_init _init_(protocol_class_init_data, sizeof(protocol_class_init_data));~%")
		  )))
		  
		  
	(map (lambda (pc)
	       (let ((obj-to-dump
		      (list (protocol-class-name pc)			; class name
			    (map car (protocol-class-incoming pc))	; incoming msg names
			    (map car (protocol-class-outgoing pc))	; outgoing msg names
			    ;;(protocol-class-incoming pc)		; full incoming msg descriptions
			    ;;(protocol-class-outgoing pc)		; full outgoing msg descriptions
			    )))	
		 ;;(pretty-print obj-to-dump)  
		 (pmt-serialize obj-to-dump (so-stream 'put-byte))))
	     classes)

	(format-output-for-c++ ((so-stream 'get-output)))

	#t))))


(define (make-serial-output-stream)
  (letrec ((output '())
	   (put-byte
	    (lambda (byte)
	      (set! output (cons byte output))))
	   (get-output
	    (lambda ()
	      (reverse output))))
    (lambda (key)
      (case key
	((put-byte) put-byte)
	((get-output) get-output)
	(else (error "Unknown key" key))))))

