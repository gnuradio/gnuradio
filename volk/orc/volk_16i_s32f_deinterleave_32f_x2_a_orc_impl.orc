.function volk_16ic_s32f_deinterleave_32f_x2_a_orc_impl
.dest 4 idst
.dest 4 qdst
.source 4 src
.floatparam 4 scalar
.temp 8 iql
.temp 8 iqf

x2 convswl iql, src
x2 convlf iqf, iql
x2 divf iqf, iqf, scalar
splitql qdst, idst, iqf
