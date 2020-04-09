/* -*- c++ -*- */

%template(adaptive_algorithm_sptr) std::shared_ptr<gr::digital::adaptive_algorithm>;

%template(adaptive_algorithm_lms_sptr) std::shared_ptr<gr::digital::adaptive_algorithm_lms>;
%pythoncode %{
adaptive_algorithm_lms_sptr.__repr__ = lambda self: "<adaptive_algorithm LMS>"
adaptive_algorithm_lms = adaptive_algorithm_lms.make;
%}
%template(adaptive_algorithm_nlms_sptr) std::shared_ptr<gr::digital::adaptive_algorithm_nlms>;
%pythoncode %{
adaptive_algorithm_nlms_sptr.__repr__ = lambda self: "<adaptive_algorithm NLMS>"
adaptive_algorithm_nlms = adaptive_algorithm_nlms.make;
%}
%template(adaptive_algorithm_cma_sptr) std::shared_ptr<gr::digital::adaptive_algorithm_cma>;
%pythoncode %{
adaptive_algorithm_cma_sptr.__repr__ = lambda self: "<adaptive_algorithm CMA>"
adaptive_algorithm_cma = adaptive_algorithm_cma.make;
%}

