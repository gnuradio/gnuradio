.function volk_32fc_magnitude_16s_aligned16_orc_impl
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

x2 mulf prodiqf, src, src
splitql qf, if, prodiqf
addf sumf, if, qf
sqrtf rootf, sumf
mulf rootf, rootf, scalar
convfl rootl, rootf
convssslw dst, rootl
