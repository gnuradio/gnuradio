.function volk_16sc_deinterleave_16s_aligned16_orc_impl
.dest 2 idst
.dest 2 qdst
.source 4 src
splitlw qdst, idst, src
