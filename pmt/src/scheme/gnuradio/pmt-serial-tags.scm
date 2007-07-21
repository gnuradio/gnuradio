;;; -*-scheme-*-
;;;
;;; Copyright 2007 Free Software Foundation, Inc.
;;; 
;;; This file is part of GNU Radio
;;; 
;;; GNU Radio is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3, or (define at your option)
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

;;; definitions of tag values used for marshalling pmt data

(define pst-true		#x00)
(define pst-false		#x01)
(define pst-symbol		#x02)   ; untagged-int16 n; followed by n bytes of symbol name
(define pst-int32		#x03)
(define pst-double		#x04)
(define pst-complex		#x05)   ; complex<double>: real, imag
(define pst-null		#x06)
(define pst-pair		#x07)   ; followed by two objects
(define pst-vector		#x08)   ; untagged-int32 n; followed by n objects
(define pst-dict		#x09)   ; untagged-int32 n; followed by n key/value tuples

(define pst-uniform-vector	#x0a)

;; u8, s8, u16, s16, u32, s32, u64, s64, f32, f64, c32, c64
;;
;;   untagged-uint8  tag
;;   untagged-uint8  uvi (define uniform vector info, see below)
;;   untagged-int32  n-items
;;   untagged-uint8  npad
;;   npad bytes of zeros to align binary data
;;   n-items binary numeric items
;;
;; uvi:
;; +-+-+-+-+-+-+-+-+
;; |B|   subtype   |
;; +-+-+-+-+-+-+-+-+
;;
;; B == 0, numeric data is little-endian.
;; B == 1, numeric data is big-endian.

    (define uvi-endian-mask     #x80)
    (define uvi-subtype-mask    #x7f)

    (define uvi-little-endian   #x00)
    (define uvi-big-endian      #x80)

    (define uvi-u8		#x00)
    (define uvi-s8		#x01)
    (define uvi-u16		#x02)
    (define uvi-s16		#x03)
    (define uvi-u32		#x04)
    (define uvi-s32		#x05)
    (define uvi-u64		#x06)
    (define uvi-s64		#x07)
    (define uvi-f32		#x08)
    (define uvi-f64		#x09)
    (define uvi-c32		#x0a)
    (define uvi-c64		#x0b)


(define pst-comment		#x3b)	; ascii ';'
(define pst-comment-end		#x0a)	; ascii '\n'
