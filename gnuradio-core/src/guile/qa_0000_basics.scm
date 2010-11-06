(use-modules (gnuradio core))
(use-modules (oop goops))

(load-from-path "srfi/srfi-64")
(use-modules (ice-9 format))
(use-modules (ice-9 pretty-print))

;; (write "Hello QA world!\n")

(define (vector-map f v)
  (list->vector (map f (vector->list v))))

(define (test-1)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "my top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using gr:ep to create endpoints
    (gr:connect tb (gr:ep src 0) (gr:ep op 0))
    (gr:connect tb (gr:ep op 0) (gr:ep dst 0))

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-2)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "my top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using just blocks
    (gr:connect tb src op)
    (gr:connect tb op dst)

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-3)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "my top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using lists to represent endpoints
    (gr:connect tb `(,src 0) `(,op 0))
    (gr:connect tb `(,op 0) `(,dst 0))

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-4)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "my top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using multiple endpoints
    (gr:connect tb src op dst)

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))


(test-begin "qa_0000_basics")
(test-1)
(test-2)
(test-3)
(test-4)
(test-end "qa_0000_basics")
