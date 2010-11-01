;;; Glue the separate pieces of gnuradio-core into a single module

(define-module (gnuradio core)
  #:use-module (gnuradio gnuradio_core_runtime)
  #:use-module (gnuradio runtime-shim)
  #:use-module (gnuradio gnuradio_core_filter)
  #:use-module (gnuradio gnuradio_core_io)
  #:use-module (gnuradio gnuradio_core_general)
  #:use-module (gnuradio gnuradio_core_gengen)
  #:use-module (gnuradio gnuradio_core_hier)
  #:duplicates (merge-generics check))

;; re-export everything...
(re-export-all (current-module))
