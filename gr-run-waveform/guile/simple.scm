(define filename "ice-9/boot-9")

;; System default path
(define path %load-path)
path
;;
(define path-with-xyzzy (cons "/-xyzzy-" path))
path-with-xyzzy
;;
;; look for .scm or no extension
(define extensions '(".scm" ""))

;; Both of these return "/usr/share/guile/1.8/ice-9/boot-9.scm"
(define expected "/usr/share/guile/1.8/ice-9/boot-9.scm")
(define result1 (search-path path filename extensions))
(if (string=? result1 expected)
    (display "PASSED: xyzzy-search-path from guile\n")
    (display "FAILED: xyzzy-search-path from guile\n"))

(define result2 (search-path path-with-xyzzy filename extensions))
(if (string=? result2 expected)
    (display "PASSED: xyzzy-search-path from guile\n")
    (display "FAILED: xyzzy-search-path from guile\n"))

;; Should return "/usr/share/guile/1.8/ice-9/boot-9.scm"
(define result3 (xyzzy-search-path path filename extensions))
(if (string=? result3 expected)
    (display "PASSED: xyzzy-search-path from guile\n")
    (display "FAILED: xyzzy-search-path from guile\n"))

;; Should return "/-xyzzy-/ice-9/boot-9.scm"
(define expected "/-xyzzy-/ice-9/boot-9.scm")
(define result4 (xyzzy-search-path path-with-xyzzy filename extensions))
(if (string=? result4 expected)
    (display "PASSED: xyzzy-search-path from guile\n")
    (display "FAILED: xyzzy-search-path from guile\n"))

;; (define result5 (primitive-load filename))

;; (define result6 (xyzzy-primitive-load file))

;; FIXME: not sure how to tell if this worked other than if the test doesn't crash
(define result7 (xyzzy-primitive-load expected))
(define result8 (xyzzy-primitive-load-path expected))

;; This should return the full name, or #f if it fails.
(define result9 (xyzzy-search-load-path filename))
(if (boolean? result9)
    (display "XFAILED: xyzzy-search-load-path from guile\n")
    (if (string=? result9 expected)
	(display "XFAILED: xyzzy-search-load-path from guile\n")
	(display "XPASSED: xyzzy-search-load-path from guile\n")))

(define result10 (xyzzy-search-load-path expected))
(if (boolean? result9)
    (display "FAILED: xyzzy-search-load-path from guile\n")
    (if (string=? result10 expected)
	(display "PASSED: xyzzy-search-load-path from guile\n")
	(display "FAILED: xyzzy-search-load-path from guile\n")))

