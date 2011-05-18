.function volk_32fc_32f_multiply_32fc_a_orc_impl
.source 8 src1
.source 4 src2
.dest 8 dst
.temp 8 tmp
mergelq tmp, src2, src2
x2 mulf dst, src1, tmp
