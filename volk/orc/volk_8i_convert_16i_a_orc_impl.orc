.function volk_8i_convert_16i_a_orc_impl
.source 1 src
.dest 2 dst
.temp 2 tmp
convsbw tmp, src
shlw dst, tmp, 8
