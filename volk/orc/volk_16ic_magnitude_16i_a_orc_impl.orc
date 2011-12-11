.function volk_16ic_magnitude_16i_a_orc_impl
.source 4 src
.dest 2 dst
.floatparam 4 scalar
.temp 8 iql
.temp 8 iqf
.temp 8 prodiqf
.temp 4 qf
.temp 4 if
.temp 4 sumf
.temp 4 rootf
.temp 4 rootl

x2 convswl iql, src
x2 convlf iqf, iql
x2 divf iqf, iqf, scalar
x2 mulf prodiqf, iqf, iqf
splitql qf, if, prodiqf
addf sumf, if, qf
sqrt, sqrtf rootf, sumf
mulf rootf, rootf, scalar
convfl rootl, rootf
convlw dst, rootl
