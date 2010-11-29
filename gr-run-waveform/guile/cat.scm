;;; This is non-idiomatic, but will exercise the port...
(define (cat input-port)
  (let loop ((ch (read-char input-port)))
      (if (not (eof-object? ch))
        (begin
              (write-char ch (current-output-port))
                  (loop (read-char input-port))))))

;; # Then start guile and use it
;; guile> (load "/tmp/cat.scm")
;; guile> (cat (open-file "/etc/passwd" "r"))

