.function volk_16ic_deinterleave_16i_x2_a_orc_impl
.dest 2 idst
.dest 2 qdst
.source 4 src
splitlw qdst, idst, src
