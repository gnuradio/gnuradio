.function volk_8s_convert_32f_aligned16_orc_impl
.source 2 src
.dest 4 dst
.floatparam 4 scalar
.temp 4 flsrc
.temp 4 lsrc
convswl lsrc, src
convlf flsrc, lsrc
mulf dst, flsrc, scalar
