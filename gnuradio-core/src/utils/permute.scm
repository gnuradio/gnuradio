(require 'common-list-functions)


(define (permute lst)
  (define (aux set head)
    (cond ((null? set) head)
	  (else
	   (map (lambda (x)
		  (aux (set-difference set (list x))
		       (cons x head)))
		set))))
  (aux lst '()))

(define (permute-2 lst)
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



