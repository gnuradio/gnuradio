.function volk_16sc_deinterleave_16s_16s_a16_orc_impl
.dest 2 idst
.dest 2 qdst
.source 4 src
splitlw qdst, idst, src
