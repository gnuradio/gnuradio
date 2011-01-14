.function volk_8s_convert_16s_aligned16_orc_impl
.source 1 src
.dest 2 dst
convsbw dst, src
shlw dst, dst, 8
