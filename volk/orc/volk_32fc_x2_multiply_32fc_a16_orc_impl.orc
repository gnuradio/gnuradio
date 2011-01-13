.function volk_32fc_x2_multiply_32fc_a16_orc_impl
.source 8 src1
.source 8 src2
.floatparam 4 mask
.dest 8 dst
.temp 8 tmp
x2 mulf dst, src1, src2
