.function volk_8i_convert_16i_a16_orc_impl
.source 1 src
.dest 2 dst
convsbw dst, src
shlw dst, dst, 8
