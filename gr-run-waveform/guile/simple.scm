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
