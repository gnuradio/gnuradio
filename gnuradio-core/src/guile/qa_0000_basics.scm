(use-modules (gnuradio core))
(use-modules (oop goops))
;;(use-modules (ice-9 format))
;;(use-modules (ice-9 pretty-print))

(load-from-path "srfi/srfi-64")		; unit test library

(define (vector-map f v)
  (list->vector (map f (vector->list v))))

(define (test-connect-1)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "QA top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using gr:ep to create endpoints
    (gr:connect tb (gr:ep src 0) (gr:ep op 0))
    (gr:connect tb (gr:ep op 0) (gr:ep dst 0))

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-connect-2)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "QA top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using just blocks
    (gr:connect tb src op)
    (gr:connect tb op dst)

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-connect-3)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "QA top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using lists to represent endpoints
    (gr:connect tb `(,src 0) `(,op 0))
    (gr:connect tb `(,op 0) `(,dst 0))

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-connect-4)
  (let* ((src-data #(-5 -4 -3 -2 -1 0 1 2 3 4 5))
	 (expected-result (vector-map (lambda (x) (* x 2)) src-data))
	 (tb (gr:top-block-swig "QA top block"))
	 (src (gr:vector-source-i src-data #f))
	 (op (gr:multiply-const-ii 2))
	 (dst (gr:vector-sink-i)))

    ;; using multiple endpoints
    (gr:connect tb src op dst)

    (gr:run tb)
    (let ((actual-result (gr:data dst)))
      (test-equal expected-result actual-result))))

(define (test-io-signature-1)
  (let ((ios1 (gr:io-signature 1 2 8))
	(ios2 (gr:io-signature2 1 2 16 32))
	(ios3 (gr:io-signature3 1 -1 14 32 48))
	(iosv (gr:io-signaturev 1 4 '(1 2 3))))

    (test-equal 1 (gr:min-streams ios1))
    (test-equal 2 (gr:max-streams ios1))
    (test-equal 8 (gr:sizeof-stream-item ios1 0))
    (test-equal 8 (gr:sizeof-stream-item ios1 1))

    (test-equal 1 (gr:min-streams ios2))
    (test-equal 2 (gr:max-streams ios2))
    (test-equal 16 (gr:sizeof-stream-item ios2 0))
    (test-equal 32 (gr:sizeof-stream-item ios2 1))

    (test-equal 1 (gr:min-streams ios3))
    (test-equal -1 (gr:max-streams ios3))
    (test-equal 14 (gr:sizeof-stream-item ios3 0))
    (test-equal 32 (gr:sizeof-stream-item ios3 1))
    (test-equal 48 (gr:sizeof-stream-item ios3 2))
    (test-equal '#(14 32 48) (gr:sizeof-stream-items ios3))

    (test-equal 1 (gr:min-streams iosv))
    (test-equal 4 (gr:max-streams iosv))
    (test-equal 1 (gr:sizeof-stream-item iosv 0))
    (test-equal 2 (gr:sizeof-stream-item iosv 1))
    (test-equal 3 (gr:sizeof-stream-item iosv 2))
    (test-equal 3 (gr:sizeof-stream-item iosv 3))
    (test-equal '#(1 2 3) (gr:sizeof-stream-items iosv))
    ))



(test-begin "qa_0000_basics")
(test-connect-1)
(test-connect-2)
(test-connect-3)
(test-connect-4)
(test-io-signature-1)
(test-end "qa_0000_basics")
