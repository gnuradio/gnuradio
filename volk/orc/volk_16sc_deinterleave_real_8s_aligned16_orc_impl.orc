.function volk_16sc_deinterleave_real_8s_aligned16_orc_impl
.dest 1 dst
.source 4 src
.temp 2 iw
select0lw iw, src
convhwb dst, iw
