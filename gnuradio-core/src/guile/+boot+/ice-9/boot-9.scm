;;; install shims for magic file system (if defined), then chain to real boot code.

(if (defined? 'xyzzy-primitive-load)
    (set! primitive-load xyzzy-primitive-load))

(if (defined? 'xyzzy-primitive-load-path)
    (set! primitive-load-path xyzzy-primitive-load-path))

(if (defined? 'xyzzy-search-path)
    (set! search-path xyzzy-search-path))

(if (defined? 'xyzzy-search-load-path)
    (set! search-load-path xyzzy-search-load-path))

;; Remove any path containing /+boot+ from %load-path
(set! %load-path
      (let ((new-path '()))
	(let loop ((path %load-path))
	  (cond ((not (pair? path))
		 (reverse! new-path))
		((string-contains (car path) "/+boot+")
		 (loop (cdr path)))
		(else
		 (set! new-path (cons (car path) new-path))
		 (loop (cdr path)))))))

(primitive-load-path "ice-9/boot-9.scm")
