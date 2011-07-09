.function volk_16ic_deinterleave_real_8i_a_orc_impl
.dest 1 dst
.source 4 src
.temp 2 iw
select0lw iw, src
convhwb dst, iw
