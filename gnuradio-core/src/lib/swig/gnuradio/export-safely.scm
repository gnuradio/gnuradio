(define-module (gnuradio export-safely)
  #:use-module (oop goops)
  #:use-module (srfi srfi-1)
  #:export-syntax (export-safely))

(define-public (generics-in-module module)
  (let ((lst '()))
    (module-for-each (lambda (sym var)
		       (if (variable-bound? var)
			   (let ((v (variable-ref var)))
			     (cond ((is-a? v <generic>)
				    (set! lst (cons v lst)))))))
		     module)
    lst))

(define-public (generic-function-names-in-module module)
  (map generic-function-name (generics-in-module module)))

(define-public (generic-function-names-in-imported-modules module)
  (concatenate (map generic-function-names-in-module (module-uses module))))

(define-public (export-syms-if-not-imported-gf list-of-syms)
  (let ((gf-names (generic-function-names-in-imported-modules (current-module))))
    (let ((to-export (filter (lambda (sym)
			       (not (memq sym gf-names)))
			     list-of-syms)))
      (module-export! (current-module) to-export))))
	 
(defmacro export-safely names
  `(export-syms-if-not-imported-gf ',names))


(define-public (names-in-module module)
  (let ((lst '()))
    (module-for-each (lambda (sym var)
		       (if (variable-bound? var)
			   (set! lst (cons sym lst))))
		     module)
    lst))

(define-public (names-in-imported-modules module)
  (concatenate (map names-in-module (module-uses module))))

(define-public (re-export-all module)
  (module-re-export! module (names-in-imported-modules module)))
