.function volk_32fc_magnitude_32f_a_orc_impl
.source 8 src
.dest 4 dst
.temp 8 iqf
.temp 8 prodiqf
.temp 4 qf
.temp 4 if
.temp 4 sumf

x2 mulf prodiqf, src, src
splitql qf, if, prodiqf
addf sumf, if, qf
sqrtf dst, sumf
