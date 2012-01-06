.function volk_32fc_s32f_magnitude_16i_a_orc_impl
.source 8 src
.dest 2 dst
.floatparam 4 scalar
.temp 8 iqf
.temp 8 prodiqf
.temp 4 qf
.temp 4 if
.temp 4 sumf
.temp 4 rootf
.temp 4 rootl
#.temp 4 maskl

x2 mulf prodiqf, src, src
splitql qf, if, prodiqf
addf sumf, if, qf
sqrtf rootf, sumf
mulf rootf, rootf, scalar
#cmpltf maskl, 32768.0, rootf
#andl maskl, maskl, 0x80000000
#orl rootf, rootf, maskl
convfl rootl, rootf
convsuslw dst, rootl
