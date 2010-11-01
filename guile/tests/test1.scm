(use-modules (gnuradio core))
(use-modules (oop goops))
(use-modules (ice-9 format))
(use-modules (ice-9 pretty-print))

(define ep gr:ep)

(define (vector-map f v)
  (list->vector (map f (vector->list v))))

(define (check-result name a b)
  (cond ((equal? a b) #t)
	(else
	 (format 1 "~a: failed:\n  expected: ~y\n  got:\n~y\n" name a b)
	 #f)))
	 

(define (test-1)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "my top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))
    (gr:connect tb src op dst)
    ;;(gr:connect tb `(,src 0) `(,op 0))
    ;;(gr:connect tb `(,op 0) `(,dst 0))
    ;;(gr:connect tb (ep src 0) (ep op 0))
    ;;(gr:connect tb (ep op 0) (ep dst 0))
    ;;(gr:connect tb src op)
    ;;(gr:connect tb op dst)
    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      ;;(pretty-print actual-result)
      (check-result 'test-1 expected-result actual-result))))

