// Put this somewhere else
#define __EXPAND(x) x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...) COUNT
#define __VA_SIZE(...) __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b) a##b
#define __CAT2(a, b) __CAT1(a, b)
#define __DOC1(n1) ""
#define __DOC2(n1, n2) ""
#define __DOC3(n1, n2, n3) ""
#define __DOC4(n1, n2, n3, n4) ""
#define __DOC5(n1, n2, n3, n4, n5) ""
#define __DOC6(n1, n2, n3, n4, n5, n6) ""
#define __DOC7(n1, n2, n3, n4, n5, n6, n7) ""
#define DOC(...) __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))
#define D(...) DOC(gr, __VA_ARGS__)
