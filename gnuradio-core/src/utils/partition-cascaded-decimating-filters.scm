;; Estimate the total work (ntaps * sampling rate) for two cascaded
;; decimating low pass filters.
;;
;; The basic assumption is that the number of taps required in any
;; section is inversely proportional to the normalized transition width
;; for that section.
;;
;; FS is the input sampling frequency
;; F1 is the cutoff frequency
;; F2 is the far edge of the transition band
;; DEC1 is the decimation factor for the first filter
;; DEC2 is the decimation factor for the 2nd filter
;;
;; The total decimation factor is DEC1 * DEC2.  Therefore,
;; the output rate of the filter is FS / (DEC1 * DEC2)

(require 'common-list-functions)
(require 'factor)



(define (work2 fs f1 f2 dec1 dec2)
  (+ (work1 fs f1 (/ fs (* 2 dec1)) dec1)
     (work1 (/ fs dec1) f1 f2 dec2)))


;; work for a single section

(define (work1 fs f1 f2 dec)
  (/ (* fs (/ fs (- f2 f1))) dec))


;; return the max integer dec such that fs/(2*dec) >= f2

(define (max-dec fs f2)
  (inexact->exact (floor (/ fs (* 2 f2)))))


;; `adjoin' returns the adjoint of the element OBJ and the list LST.
;;  That is, if OBJ is in LST, `adjoin' returns LST, otherwise, it returns
;;  `(cons OBJ LST)'.

(define (adjoin-equal obj lst)
  (if (member obj lst) lst (cons obj lst)))


;;; not quite right

(define (permute lst)
  (let ((result '()))
    (define (aux set head)
      (if (null? set)
	  (set! result (cons head result))
	  (for-each (lambda (x)
		      (aux (set-difference set (list x))
			   (cons x head)))
		    set)))
    (aux lst '())
    result))

;; `extract-nth' returns the Nth element of LST consed on to the
;; list resulting from splicing out the Nth element of LST.
;; Indexing is 0 based.

(define (extract-nth n lst)
  lst)

