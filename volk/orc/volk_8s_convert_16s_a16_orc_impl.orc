.function volk_8s_convert_16s_a16_orc_impl
.source 1 src
.dest 2 dst
convsbw dst, src
shlw dst, dst, 8
