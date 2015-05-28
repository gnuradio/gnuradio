#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

from encoder import PolarEncoder as enc

# input alphabet X is always {0,1}
# output alphabet is arbitrary
# transition probabilities are arbitrary W(y|x)


def bit_reverse(value, n):
    # is this really missing in NumPy???
    bits = np.zeros(n, type(value))
    for index in range(n):
        mask = 1
        mask = np.left_shift(mask, index)
        bit = np.bitwise_and(value, mask)
        bit = np.right_shift(bit, index)
        bits[index] = bit
    bits = bits[::-1]
    result = 0
    for index, bit in enumerate(bits):
        bit = np.left_shift(bit, index)
        result += bit
    return result


def get_Bn(n):
    # this is a bit reversal matrix.
    lw = int(np.log2(n))  # number of used bits
    indexes = [bit_reverse(i, lw) for i in range(n)]
    Bn = np.zeros((n, n), type(n))
    for i, index in enumerate(indexes):
        Bn[i][index] = 1
    return Bn


def get_Fn(n):
    # this matrix defines the actual channel combining.
    if n == 1:
        return np.array([1, ])
    F2 = np.array([[1, 0], [1, 1]], np.int)
    nump = int(np.log2(n)) - 1  # number of Kronecker products to calculate
    Fn = F2
    for i in range(nump):
        Fn = np.kron(Fn, F2)
    return Fn

def get_Gn(n):
    # this matrix is called generator matrix
    if not is_power_of_2(n):
        print "invalid input"
        return None
    if n == 1:
        return np.array([1, ])
    Bn = get_Bn(n)
    Fn = get_Fn(n)
    Gn = np.dot(Bn, Fn)
    return Gn


def odd_rec(iwn):
    return iwn ** 2


def even_rec(iwn):
    return 2 * iwn - iwn ** 2


def calc_one_recursion(iw0):
    iw1 = np.zeros(2 * len(iw0))  # double values
    for i in range(len(iw0)):
        # careful indices screw you because paper is '1' based :(
        iw1[2 * i] = odd_rec(iw0[i])
        iw1[2 * i + 1] = even_rec(iw0[i])
    return iw1


def calculate_bec_channel_capacities(eta, n):
    iw = 1 - eta  # holds for BEC as stated in paper
    lw = int(np.log2(n))
    iw = [iw, ]
    for i in range(lw):
        iw = calc_one_recursion(iw)
    return iw


def bsc_channel(p):
    '''
    binary symmetric channel (BSC)
    output alphabet Y = {0, 1} and
    W(0|0) = W(1|1) and W(1|0) = W(0|1)

    this function returns a prob's vector for a BSC
    p denotes an erroneous transistion
    '''
    if not (p >= 0.0 and p <= 1.0):
        print "given p is out of range!"
        return ()

    # 0 -> 0, 0 -> 1, 1 -> 0, 1 -> 1
    W = np.array([[1 - p, p], [p, 1 - p]])
    return W


def bec_channel(eta):
    '''
    binary erasure channel (BEC)
    for each y e Y
    W(y|0) * W(y|1) = 0 or W(y|0) = W(y|1)
    transistions are 1 -> 1 or 0 -> 0 or {0, 1} -> ? (erased symbol)
    '''
    print eta

    # looks like BSC but should be interpreted differently.
    W = (1 - eta, eta, 1 - eta)
    return W


def is_power_of_2(num):
    if type(num) != int:
        return False  # make sure we only compute integers.
    return num != 0 and ((num & (num - 1)) == 0)


def combine_W2(u):
    # This function applies the channel combination for W2
    x1 = (u[0] + u[1]) % 2
    x2 = (u[1])
    return np.array([x1, x2], np.int)


def combine_W4(u):
    im = np.concatenate((combine_W2(u[0:2]), combine_W2(u[2:4])))
    print "combine_W4.im = ", im
    swappy = im[1]
    im[1] = im[2]
    im[2] = swappy
    print "combine_W4.reverse_shuffled = ", im

    return np.concatenate((combine_W2(im[0:2]), combine_W2(im[2:4])))


def combine_Wn(u):
    '''Combine vector u for encoding. It's described in the "Channel combining" section'''
    # will throw error if len(u) isn't a power of 2!
    n = len(u)
    G = get_Gn(n)
    return np.dot(u, G) % 2


def unpack_byte(byte, nactive):
    if np.amin(byte) < 0 or np.amax(byte) > 255:
        return None
    if not byte.dtype == np.uint8:
        byte = byte.astype(np.uint8)
    if nactive == 0:
        return np.array([], dtype=np.uint8)
    return np.unpackbits(byte)[-nactive:]


def pack_byte(bits):
    if len(bits) == 0:
        return 0
    if np.amin(bits) < 0 or np.amax(bits) > 1:  # only '1' and '0' in bits array allowed!
        return None
    bits = np.concatenate((np.zeros(8 - len(bits), dtype=np.uint8), bits))
    res = np.packbits(bits)[0]
    return res


def calculate_conditional_prob(y, u, channel):
    # y and u same size and 1d!
    # channel 2 x 2 matrix!
    x1 = (u[0] + u[1]) % 2
    s = 0 if y[0] == x1 else 1
    # print "W(", y[0], "|", u[0], "+", u[1], "=", s, ") =", channel[y[0]][x1]
    w112 = channel[y[0]][x1]
    w22 = channel[y[1]][u[1]]
    return w112 * w22


def calculate_w2_probs():
    w0 = np.array([[0.9, 0.1], [0.1, 0.9]])
    print w0

    w2 = np.zeros((4, 4), dtype=float)

    print "W(y1|u1+u2)"
    for y in range(4):
        ybits = unpack_byte(np.array([y, ]), 2)
        for u in range(4):
            ubits = unpack_byte(np.array([u, ]), 2)
            prob = calculate_conditional_prob(ybits, ubits, w0)
            w2[y][u] = prob
            # print "W(y1,y2|u1,u2) =", prob
    return w2


def get_prob(y, u, channel):
    return channel[y][u]


def get_wn_prob(y, u, channel):
    x = combine_Wn(u)
    result = 1
    for i in range(len(x)):
        result *= get_prob(y[i], x[i], channel)
    return result


def calculate_Wn_probs(n, channel):
    # print "calculate_wn_probs"
    ncomb = 2 ** n
    wn = np.ones((ncomb, ncomb), dtype=float)
    for y in range(ncomb):
        ybits = unpack_byte(np.array([y, ]), n)
        for u in range(ncomb):
            ubits = unpack_byte(np.array([u, ]), n)
            xbits = combine_Wn(ubits)
            wn[y][u] = get_wn_prob(ybits, ubits, channel)
    return wn


def calculate_channel_splitting_probs(wn, n):
    print wn

    wi = np.zeros((n, 2 ** n, 2 ** n), dtype=float)
    divider = (1.0 / (2 ** (n - 1)))
    for i in range(n):
        for y in range(2 ** n):
            ybits = unpack_byte(np.array([y, ]), n)
            print
            for u in range(2 ** n):
                ubits = unpack_byte(np.array([u, ]), n)
                usc = ubits[0:i]
                u = ubits[i]
                usum = ubits[i+1:]
                fixed_pu = np.append(usc, u)

                my_iter = []
                if len(usum) == 0:
                    my_iter.append(np.append(np.zeros(8 - len(fixed_pu), dtype=np.uint8), fixed_pu))
                else:
                    uiter = np.arange(2 ** len(usum), dtype=np.uint8)
                    for ui in uiter:
                        element = unpack_byte(ui, len(usum))
                        item = np.append(fixed_pu, element)
                        item = np.append(np.zeros(8 - len(item), dtype=np.uint8), item)
                        my_iter.append(item)
                my_iter = np.array(my_iter)

                prob_sum = 0
                for item in my_iter:
                    upos = np.packbits(item)
                    # print "y=", np.binary_repr(y, n), "item=", item, "u=", np.binary_repr(upos, n)
                    wni = wn[y][upos]
                    prob_sum += wni
                prob_sum *= divider

                print "W[{5}]({0},{1},{2}|{3}) = {4}".format(ybits[0], ybits[1], usc, u, prob_sum, i)

                # print "i=", i, "y=", np.binary_repr(y, n), " fixed=", fixed_pu, "usum=", usum, " WN(i) =", prob_sum
                wi[i][y][u] = prob_sum

    for i in range(n):
        print
        for y in range(2 ** n):
            ybits = unpack_byte(np.array([y, ]), n)
            for u in range(2 ** n):

                print "W[{}] ({},{}|{})".format(i, ybits[0], ybits[1], u)



    return wi



def mutual_information(w):
    '''
    calculate mutual information I(W)
    I(W) = sum over y e Y ( sum over x e X ( ... ) )
    .5 W(y|x) log frac { W(y|x) }{ .5 W(y|0) + .5 W(y|1) }
    '''
    ydim, xdim = np.shape(w)
    i = 0.0
    for y in range(ydim):
        for x in range(xdim):
            v = w[y][x] * np.log2(w[y][x] / (0.5 * w[y][0] + 0.5 * w[y][1]))
            i += v
    i /= 2.0
    return i


def capacity(w):
    '''
    find supremum I(W) of a channel.
    '''
    return w


def bhattacharyya_parameter(w):
    '''bhattacharyya parameter is a measure of similarity between two prob. distributions'''
    # sum over all y e Y for sqrt( W(y|0) * W(y|1) )
    dim = np.shape(w)
    ydim = dim[0]
    xdim = dim[1]
    z = 0.0
    for y in range(ydim):
        z += np.sqrt(w[y][0] * w[y][1])
    # need all
    return z


def w_transition_prob(y, u, p):
    return 1 - p if y == u else p


def w_split_prob(y, u, G, p):
    ''' Calculate channel splitting probabilities. '''
    N = len(y)  # number of combined channels
    df = N - len(u)  # degrees of freedom.
    prob = 0.0
    for uf in range(2 ** df):
        utemp = unpack_byte(np.array([uf, ]), df)
        ub = np.concatenate((u, utemp))
        # print "y=", y, "\tu=", ub
        x = np.dot(ub, G) % 2
        # print "x=", x
        w = 1.0
        for i in range(N):
            w *= w_transition_prob(y[i], x[i], p)
        # print "for u1=", uf, "prob=", w
        prob += w
    divider = 1.0 / (2 ** (N - 1))
    return divider * prob


def wn_split_channel(N, p):
    G = get_Gn(N)
    y = np.zeros((N, ), dtype=np.uint8)
    n = 1
    u = np.zeros((n + 1, ), dtype=np.uint8)

    pn = w_split_prob(y, u, G, p)
    # print "pn=", pn
    z_params = []
    c_params = []
    for w in range(N):
        nentries = (2 ** N) * (2 ** w)
        print "for w=", w, " nentries=", nentries
        w_probs = np.zeros((nentries, 2), dtype=float)
        for y in range(2 ** N):
            yb = unpack_byte(np.array([y, ]), N)
            # print "\n\nyb", yb
            for u in range(2 ** (w + 1)):
                ub = unpack_byte(np.array([u, ]), w + 1)
                # print "ub", ub
                wp = w_split_prob(yb, ub, G, p)
                ufixed = ub[0:-1]
                yindex = y * (2 ** w) + pack_byte(ufixed)
                uindex = ub[-1]
                # print "y=", y, "ub", u, " prob=", wp, "index=[", yindex, ", ", uindex, "]"
                w_probs[yindex][uindex] = wp
        print "\n", np.sum(w_probs, axis=0)
        z = bhattacharyya_parameter(w_probs)
        z_params.append(z)
        c = mutual_information(w_probs)
        c_params.append(c)
        print "W=", w, "Z=", z, "capacity=", c

    return z_params, c_params


def wminus(y0, y1, u0, p):
    prob = 0.0
    for i in range(2):
        # print y0, y1, u0, i
        u0u1 = (i + u0) % 2
        w0 = w_transition_prob(y0, u0u1, p)
        w1 = w_transition_prob(y1, i, p)
        # print "w0=", w0, "\tw1=", w1
        prob += w0 * w1
    prob *= 0.5
    return prob


def wplus(y0, y1, u0, u1, p):
    u0u1 = (u0 + u1) % 2
    w0 = w_transition_prob(y0, u0u1, p)
    w1 = w_transition_prob(y1, u1, p)
    return 0.5 * w0 * w1


def w2_split_channel(p):

    wm_probs = np.zeros((4, 2), dtype=float)
    for y0 in range(2):
        for y1 in range(2):
            for u0 in range(2):
                wm = wminus(y0, y1, u0, p)
                wm_probs[y0 * 2 + y1][u0] = wm

    print wm_probs
    print "W- Z parameter=", bhattacharyya_parameter(wm_probs)
    print "I(W-)=", mutual_information(wm_probs)

    chan_prob = 0.0
    wp_probs = np.zeros((8, 2), dtype=float)
    for y0 in range(2):
        for y1 in range(2):
            for u0 in range(2):
                for u1 in range(2):
                    wp = wplus(y0, y1, u0, u1, p)
                    wp_probs[4 * y0 + 2 * y1 + u0][u1] = wp

    print wp_probs
    print "W+ Z parameter=", bhattacharyya_parameter(wp_probs)
    print "I(W+)=", mutual_information(wp_probs)


def plot_channel(probs, p, name):
    nc = len(probs)
    plt.plot(probs)
    plt.grid()
    plt.xlim((0, nc - 1))
    plt.ylim((0.0, 1.0))
    plt.xlabel('channel')
    plt.ylabel('capacity')

    fname = name, '_', p, '_', nc
    print fname
    fname = ''.join(str(n) for n in fname)
    print fname
    fname = fname.replace('.', '_')
    print fname
    fname = ''.join((fname, '.pdf'))
    print fname
    plt.savefig(fname)
    # plt.show()


def main():
    np.set_printoptions(precision=4, linewidth=150)
    print "POLAR code!"
    w2 = calculate_w2_probs()
    print w2

    p = 0.1
    n = 2
    wn = calculate_Wn_probs(n, bsc_channel(p))

    wi = calculate_channel_splitting_probs(wn, n)

    w0 = bsc_channel(p)
    print w0
    iw = mutual_information(w0)
    print "I(W)=", iw
    print 1 - (p * np.log2(1.0 / p) + (1 - p) * np.log2(1.0 / (1 - p)))

    print "Z param call"
    bhattacharyya_parameter(w0)

    print "old vs new encoder"

    p = 0.1
    w2_split_channel(p)
    z, c = wn_split_channel(4, p)

    eta = 0.3
    nc = 1024
    probs = calculate_bec_channel_capacities(eta, nc)
    # plot_channel(probs, eta, 'bec_capacity')
    # plot_channel(c, p, 'bsc_capacity')

    nt = 2 ** 5


    fG = get_Gn(nt)
    # print fG
    encoder = enc(nt, 4, np.arange(nt - 4, dtype=int))
    # print encoder.get_gn()
    comp = np.equal(fG, encoder.get_gn())

    print "is equal?", np.all(comp)



if __name__ == '__main__':
    main()
