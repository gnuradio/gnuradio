.function volk_16sc_magnitude_32f_aligned16_orc_impl
.source 4 src
.dest 4 dst
.floatparam 4 scalar
.temp 4 invscalar
.temp 4 reall
.temp 4 imagl
.temp 2 reals
.temp 2 imags
.temp 4 realf
.temp 4 imagf
.temp 4 sumf

divf invscalar, 1.0, scalar
splitlw reals, imags, src
convswl reall, reals
convswl imagl, imags
convlf realf, reall
convlf imagf, imagl
mulf realf, realf, invscalar
mulf imagf, imagf, invscalar
mulf realf, realf, realf
mulf imagf, imagf, imagf
addf sumf, realf, imagf
sqrtf dst, sumf
