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

;;; An implementation of pmt_serialize in scheme.
;;; Currently handles only symbols and pairs.  They're all we need for now.

(define-module (gnuradio pmt-serialize)
  :export (pmt-serialize))

(load-from-path "gnuradio/pmt-serial-tags")

(define (pmt-serialize obj put-byte)
  (define (put-u16 x)
    (put-byte (logand (ash x -8) #xff))
    (put-byte (logand x #xff)))
    
  (cond ((null? obj)
	 (put-byte pst-null))
	((symbol? obj)
	 (let* ((sym-as-bytes (map char->integer (string->list (symbol->string obj))))
		(len (length sym-as-bytes)))
	   (put-byte pst-symbol)
	   (put-u16 len)
	   (for-each put-byte sym-as-bytes)))
	
	((pair? obj)
	 (put-byte pst-pair)
	 (pmt-serialize (car obj) put-byte)
	 (pmt-serialize (cdr obj) put-byte))
	(else
	 (throw 'not-implemented "pmt-serialize" obj))))
