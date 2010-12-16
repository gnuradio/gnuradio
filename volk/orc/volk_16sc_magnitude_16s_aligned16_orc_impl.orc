.function volk_16sc_magnitude_16s_aligned16_orc_impl
.source 4 src
.dest 2 dst
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
convwl reall, reals
convwl imagl, imags
convlf realf, reall
convlf imagf, imagl
mulf realf, realf, (1.0 / 32768.0)
mulf imagf, imagf, (1.0 / 32768.0)
addf sumf, realf, imagf
sqrtf rootf, sumf
convfl rootl, rootf
conflw dst, rootl
