.function volk_32fc_s32fc_multiply_32fc_a_orc_impl
.source 8 src1
.floatparam 8 scalar
.dest 8 dst
.temp 8 iqprod
.temp 4 real
.temp 4 imag
.temp 4 ac
.temp 4 bd
.temp 8 swapped
x2 mulf iqprod, src1, scalar
splitql bd, ac, iqprod
subf real, ac, bd
swaplq swapped, src1
x2 mulf iqprod, swapped, scalar
splitql bd, ac, iqprod
addf imag, ac, bd
mergelq dst, real, imag
