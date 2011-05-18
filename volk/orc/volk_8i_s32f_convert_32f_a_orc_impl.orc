.function volk_8i_s32f_convert_32f_a_orc_impl
.source 1 src
.dest 4 dst
.floatparam 4 scalar
.temp 4 flsrc
.temp 4 lsrc
.temp 2 ssrc
convsbw ssrc, src
convswl lsrc, ssrc
convlf flsrc, lsrc
mulf dst, flsrc, scalar
