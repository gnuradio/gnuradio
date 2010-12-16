.function volk_32fc_magnitude_16s_aligned16_orc_impl
.source 8 src
.dest 2 dst
.floatparam 4 scalar
.temp 4 invscalar
.temp 4 reall
.temp 4 imagl
.temp 4 realf
.temp 4 imagf
.temp 4 sumf
.temp 4 rootf
.temp 4 rootl

divf invscalar, 1.0, scalar
splitql reall, imagl, src
convlf realf, reall
convlf imagf, imagl
mulf realf, realf, invscalar
mulf imagf, imagf, invscalar
mulf realf, realf, realf
mulf imagf, imagf, imagf
addf sumf, realf, imagf
sqrtf rootf, sumf
convfl rootl, rootf
convlw dst, rootl
