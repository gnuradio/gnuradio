(define-module (gnuradio core)
  #:use-module (gnuradio gnuradio_core_runtime)
  #:use-module (gnuradio gnuradio_core_filter)
  #:use-module (gnuradio gnuradio_core_io)
  #:use-module (gnuradio gnuradio_core_general)
  #:use-module (gnuradio gnuradio_core_gengen)
  #:use-module (gnuradio gnuradio_core_hier)
  #:duplicates (merge-generics replace warn-override-core warn last))

;; re-export everything...

