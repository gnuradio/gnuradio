.function volk_16sc_magnitude_16s_aligned16_orc_impl
.source 4 src
.dest 2 dst
.floatparam 4 scalar
.temp 2 reals
.temp 2 imags
.temp 4 reall
.temp 4 imagl
.temp 4 realf
.temp 4 imagf
.temp 4 sumf
.temp 4 rootf
.temp 4 rootl

splitlw reals, imags, src
convswl reall, reals
convswl imagl, imags
convlf realf, reall
convlf imagf, imagl
divf realf, realf, scalar
divf imagf, imagf, scalar
mulf realf, realf, realf
mulf imagf, imagf, imagf
addf sumf, realf, imagf
sqrtf rootf, sumf
mulf rootf, rootf, scalar
convfl rootl, rootf
convlw dst, rootl
