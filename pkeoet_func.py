from pypbc import *
import hashlib
import time
import argparse

from utils.logger import Logger
from utils.osutil import *


stored_params = """type a
    q 8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791
    h 12016012264891146079388821366740534204802954401251311822919615131047207289359704531102844802183906537786776
    r 730750818665451621361119245571504901405976559617
    exp2 159
    exp1 107
    sign1 1
    sign0 1
    """

params = Parameters(param_string=stored_params)
pairing = Pairing(params)

zeta = Element(pairing, G2)
alpha = Element(pairing, G2)
g = Element(pairing, G1)
h = Element(pairing, G1)
one = Element.one(pairing, Zr)


def main(args):
    global zeta, alpha, g, h, one

    total_time = 0.0

    g, h, zeta, setup_time = setup()
    total_time += setup_time

    dpk, alpha, dkg_time = dkg()
    total_time += dkg_time

    sk1, pk1, ukg_time = ukg()
    tk1, tkg_time = tkg(dpk, sk1)
    total_time += ukg_time + tkg_time

    sk2, pk2, ut2 = ukg()
    tk2, tt2 = tkg(dpk, sk2)

    logger = Logger(join('./output', 'log1.txt'), title="PKEOET")
    logger.set_names(['Cipher Num', 'Enc Time', 'PTest Time', 'Total Time'])

    nums1 = [10, 20, 30, 40, 50, 100, 150, 200]
    nums2 = [i*100 for i in range(3, 1000)]
    nums = nums1 + nums2

    count = 0
    cursor = 0
    enc_time = 0
    pTest_time = 0
    with open('./ciphertexts/c_gt.txt', 'r') as f:
        for l in f.readlines():
            count += 1
            m = Element(pairing, GT, value=l.strip())

            c1, et1 = enc(pk1, tk1, sk1, m)
            c2, et2 = enc(pk2, tk2, sk2, m)
            enc_time += et1

            v, pt = pTest(c1, c2, pk1, pk2, tk1, tk2, dpk)
            pTest_time += pt
            total_time += et1 + pt

            if count == nums[cursor]:
                print('Enc %d Time: %s' % (count, enc_time))
                print('pTest %d Time: %s' % (count, pTest_time))
                print('Total Time: ', total_time)
                logger.append([count, enc_time, pTest_time, total_time])
                cursor += 1

    count = 0
    for i in range(len(v)):
        if dTest(v[i]) == 1:
            count += 1
    print(count)

def setup():
    start = time.time()

    g = Element.random(pairing, G1)
    h = Element.random(pairing, G1)
    zeta = Element.random(pairing, G2)

    setup_time = time.time() - start
    
    return g, h, zeta, setup_time

def init(num):
    t_m, m = [], []
    for i in range(num):
        m.append(Element(pairing, GT))
        t_m.append(Element.random(pairing, G2))
        
    m = [pairing.apply(g, t_m[i]) for i in range(num)]
    
    return m

def dkg():
    start = time.time()

    dpk = Element(pairing, G2)
    
    alpha = Element.random(pairing, Zr)
    dpk = zeta**alpha

    dkg_time = time.time() - start
    
    return dpk, alpha, dkg_time

def ukg():
    start = time.time()

    sk, pk = [], []
    for i in range(6):
        sk.append(Element.random(pairing, Zr))

    pk = [(g**sk[i]) * (h**sk[i+1]) for i in range(0, 6, 2)]

    ukg_time = time.time() - start
    
    return sk, pk, ukg_time

def tkg(tdpk, tsk):
    start = time.time()

    tk = []
    for i in range(6):
        tk.append(Element(pairing, G2))
    
    tk = [tdpk**tsk[i] for i in range(6)]

    tkg_time = time.time() - start
    
    return tk, tkg_time

def get_hash(c1, c2, c3):
    h_in = str(c1) + str(c2) + str(c3)
    sha512 = hashlib.sha512()
    sha512.update(h_in.encode('utf8'))
    hv = sha512.hexdigest()
    ev = Element.from_hash(pairing, Zr, hv)
    
    return ev

def IVgen(tc1, tc2, ttk1, ttk2):
    omega1 = Element.random(pairing, Zr)
    omega2 = Element.random(pairing, Zr)

    w1_1 = tc1[0]
    w2_1 = tc1[1]
    x_1 = tc1[2]
    y_1 = tc1[3]

    w1_2 = tc2[0]
    w2_2 = tc2[1]
    x_2 = tc2[2]
    y_2 = tc2[3]

    v1 = (x_1 * (x_2**(-one)))**omega1

    e1 = pairing.apply(w1_1, ttk1[0])
    e2 = pairing.apply(w2_1, ttk1[1])
    e3 = pairing.apply(w1_2, ttk2[0])
    e4 = pairing.apply(w2_2, ttk2[1])

    v2 = ((e1 * e2) * ((e3 * e4)**(-one)))**omega1

    v3 = (y_1 * (y_2**(-one)))**omega2

    theta1 = get_hash(w1_1, w2_1, x_1)
    theta2 = get_hash(w1_2, w2_2, x_2)

    e5 = pairing.apply(w1_1, ttk1[2])
    e6 = pairing.apply(w2_1, ttk1[3])
    e7 = (pairing.apply(w1_1, ttk1[4]))
    e8 = (pairing.apply(w2_1, ttk1[5]))
    temp_y1 = e5 * e6 * (e7**theta1) * (e8**theta1)

    e9 = pairing.apply(w1_2, ttk2[2])
    e10 = pairing.apply(w2_2, ttk2[3])
    e11 = (pairing.apply(w1_2, ttk2[4]))
    e12 = (pairing.apply(w2_2, ttk2[5]))
    temp_y2 = e9 * e10 * (e11**theta2) * (e12**theta2)

    v4 = (temp_y1 * (temp_y2**(-one)))**omega2

    return [v1, v2, v3, v4]

def enc(tpk, ttk, tsk, tm):
    start = time.time()

    r = Element.random(pairing, Zr)
    
    w1 = g**r
    w2 = h**r
    e = pairing.apply(tpk[0]**r, zeta)
    x = (pairing.apply(tpk[0]**r, zeta)) * tm  # e((g^s * h^t)^r, zeta)
    
    e1 = pairing.apply(w1, ttk[0])
    e2 = pairing.apply(w2, ttk[1])
    
    theta = get_hash(w1, w2, x)
    y = pairing.apply((tpk[1]**r) * (tpk[2]**theta)**r, zeta)
    c = [w1, w2, x, y]
    
    enc_time = time.time() - start
    
    return c, enc_time

def dec(tsk, tc):
    m = []
    one = Element.one(pairing, Zr)
    
    for i in range(len(tc)):
        w1 = tc[i][0]
        w2 = tc[i][1]
        x = tc[i][2]
        y = tc[i][3]
        
        theta = get_hash(w1, w2, x)
        ty1 = (w1**(tsk[2] + theta*tsk[4]))
        ty2 = (w2**(tsk[3] + theta*tsk[5]))
        e1 = pairing.apply((ty1 * ty2), zeta)
        
        if y != e1:
            return None
        else:
            tx = (w1**tsk[0]) * (w2**tsk[1])
            e2 = pairing.apply(tx, zeta)
            m.append(x * (e2**(-one)))
    
    return m

def pTest(tc1, tc2, tpk1, tpk2, ttk1, ttk2, tdpk):
    start = time.time()

    e1 = pairing.apply(tpk1[0], tdpk)
    e2 = pairing.apply(g, ttk1[0])
    e3 = pairing.apply(h, ttk1[1])
    b1 = (e1 == (e2 * e3))

    e4 = pairing.apply(tpk1[1], tdpk)
    e5 = pairing.apply(g, ttk1[2])
    e6 = pairing.apply(h, ttk1[3])
    b2 = (e4 == (e5 * e6))

    e7 = pairing.apply(tpk1[2], tdpk)
    e8 = pairing.apply(g, ttk1[4])
    e9 = pairing.apply(h, ttk1[5])
    b3 = (e7 == (e8 * e9))

    e10 = pairing.apply(tpk2[0], tdpk)
    e11 = pairing.apply(g, ttk2[0])
    e12 = pairing.apply(h, ttk2[1])
    b4 = (e10 == (e11 * e12))

    e13 = pairing.apply(tpk2[1], tdpk)
    e14 = pairing.apply(g, ttk2[2])
    e15 = pairing.apply(h, ttk2[3])
    b5 = (e13 == (e14 * e15))

    e16 = pairing.apply(tpk2[2], tdpk)
    e17 = pairing.apply(g, ttk2[4])
    e18 = pairing.apply(h, ttk2[5])
    b6 = (e16 == (e17 * e18))

    b7 = (b1 and b2 and b3 and b4 and b5 and b6)
    if b7:
        v = IVgen(tc1, tc2, ttk1, ttk2)
    else:
        return None

    ptest_time = time.time() - start
     
    return v, ptest_time

def dTest(tv):
    if((tv[2]**alpha) != tv[3]):
        return None
    else:
        if((tv[0]**alpha) != tv[1]):
            return 0
        else:
            return 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='PKEOET')
    parser.add_argument('--num', type=int, default=10, metavar='N',
                        help='number of chiphertext generated under each pk')

    main(parser.parse_args())
