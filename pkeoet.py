from pypbc import *
import hashlib
import time

import numpy as np
import json


class MyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, np.ndarray):
            return obj.tolist()
        else:
            return super(MyEncoder, self).default(obj)


class PKEOET(object):
    def __init__(self, pairing, num):
        super(PKEOET, self).__init__()
        self.pairing = pairing

        self.num = num
        self.zeta = Element(self.pairing, G2)
        self.alpha = Element(self.pairing, Zr)
        self.dpk = Element(self.pairing, G2)

        self.g = Element(self.pairing, G1)
        self.h = Element(self.pairing, G1)
        self.one = Element.one(self.pairing, Zr)

        self.info = {}


    def setup(self):
        start = time.time()

        self.g = Element.random(self.pairing, G1)
        self.h = Element.random(self.pairing, G1)
        self.zeta = Element.random(self.pairing, G2)

        setup_time = time.time() - start
        self.total_time += setup_time
        
        return setup_time

    def init(self):
        t_m, m = [], []
        for i in range(self.num):
            m.append(Element(self.pairing, GT))
            t_m.append(Element.random(self.pairing, G2))

        m = [self.pairing.apply(self.g, t_m[i]) for i in range(self.num)]

        return m
    
    def dkg(self):
        start = time.time()

        self.dpk = Element(self.pairing, G2)

        self.alpha = Element.random(self.pairing, Zr)
        self.dpk = self.zeta**self.alpha

        dkg_time = time.time() - start
        
        return dkg_time

    def ukg(self):
        start = time.time()

        sk, pk = [], []
        for i in range(6):
            sk.append(Element.random(self.pairing, Zr))
    
        pk = [(self.g**sk[i]) * (self.h**sk[i+1]) for i in range(0, 6, 2)]
    
        ukg_time = time.time() - start
        
        return ukg_time
        
        return sk, pk

    def tkg(self, tsk):
        start = time.time()

        tk = []
        for i in range(6):
            tk.append(Element(self.pairing, G2))

        tk = [self.dpk**tsk[i] for i in range(6)]

        tkg_time = time.time() - start

        return tk, tkg_time

    def get_hash(self, c1, c2, c3):
        h_in = str(c1) + str(c2) + str(c3)
        sha512 = hashlib.sha512()
        sha512.update(h_in.encode('utf8'))
        hv = sha512.hexdigest()
        ev = Element.from_hash(self.pairing, Zr, hv)

        return ev

    def IVgen(self, tc1, tc2, ttk1, ttk2):
        omega1 = Element.random(self.pairing, Zr)
        omega2 = Element.random(self.pairing, Zr)

        w1_1 = tc1[0]
        w2_1 = tc1[1]
        x_1 = tc1[2]
        y_1 = tc1[3]

        w1_2 = tc2[0]
        w2_2 = tc2[1]
        x_2 = tc2[2]
        y_2 = tc2[3]

        v1 = (x_1 * (x_2**(-self.one)))**omega1

        e1 = self.pairing.apply(w1_1, ttk1[0])
        e2 = self.pairing.apply(w2_1, ttk1[1])
        e3 = self.pairing.apply(w1_2, ttk2[0])
        e4 = self.pairing.apply(w2_2, ttk2[1])

        v2 = ((e1 * e2) * ((e3 * e4)**(-self.one)))**omega1

        v3 = (y_1 * (y_2**(-self.one)))**omega2

        theta1 = self.get_hash(w1_1, w2_1, x_1)
        theta2 = self.get_hash(w1_2, w2_2, x_2)

        e5 = self.pairing.apply(w1_1, ttk1[2])
        e6 = self.pairing.apply(w2_1, ttk1[3])
        e7 = (self.pairing.apply(w1_1, ttk1[4]))
        e8 = (self.pairing.apply(w2_1, ttk1[5]))
        temp_y1 = e5 * e6 * (e7**theta1) * (e8**theta1)

        e9 = self.pairing.apply(w1_2, ttk2[2])
        e10 = self.pairing.apply(w2_2, ttk2[3])
        e11 = (self.pairing.apply(w1_2, ttk2[4]))
        e12 = (self.pairing.apply(w2_2, ttk2[5]))
        temp_y2 = e9 * e10 * (e11**theta2) * (e12**theta2)

        v4 = (temp_y1 * (temp_y2**(-self.one)))**omega2
        # print(v3**self.alpha == v4)

        return [v1, v2, v3, v4]

    def enc(self, tpk, ttk, tsk, tm):
        start = time.time()

        c = []
        for i in range(self.num):
            r = Element.random(self.pairing, Zr)

            w1 = self.g**r
            w2 = self.h**r

            e = self.pairing.apply(tpk[0]**r, self.zeta)
            x = (self.pairing.apply(tpk[0]**r, self.zeta)) * tm[i] # e((g^s * h^t)^r, zeta)

            e1 = self.pairing.apply(w1, ttk[0])
            e2 = self.pairing.apply(w2, ttk[1])

            theta = self.get_hash(w1, w2, x)
            y = self.pairing.apply((tpk[1]**r) * (tpk[2]**theta)**r, self.zeta)

            c.append([w1, w2, x, y])

        enc_time = time.time() - start

        return c, enc_time

    def dec(self, tsk, tc):
        m = []

        for i in range(len(tc)):
            w1 = tc[i][0]
            w2 = tc[i][1]
            x = tc[i][2]
            y = tc[i][3]

            theta = self.get_hash(w1, w2, x)
            ty1 = (w1**(tsk[2] + theta*tsk[4]))
            ty2 = (w2**(tsk[3] + theta*tsk[5]))
            e1 = self.pairing.apply((ty1 * ty2), self.zeta)

            if y != e1:
                return None
            else:
                tx = (w1**tsk[0]) * (w2**tsk[1])
                e2 = self.pairing.apply(tx, self.zeta)
                m.append(x * (e2**(-self.one)))

        return m

    def pTest(self, tc1, tc2, tpk1, tpk2, ttk1, ttk2):
        start = time.time()

        v = []
        for i in range(len(tc1)):
            for j in range(len(tc2)):
                print(i, j)
                e1 = self.pairing.apply(tpk1[0], self.dpk)
                e2 = self.pairing.apply(self.g, ttk1[0])
                e3 = self.pairing.apply(self.h, ttk1[1])
                b1 = (e1 == (e2 * e3))

                e4 = self.pairing.apply(tpk1[1], self.dpk)
                e5 = self.pairing.apply(self.g, ttk1[2])
                e6 = self.pairing.apply(self.h, ttk1[3])
                b2 = (e4 == (e5 * e6))

                e7 = self.pairing.apply(tpk1[2], self.dpk)
                e8 = self.pairing.apply(self.g, ttk1[4])
                e9 = self.pairing.apply(self.h, ttk1[5])
                b3 = (e7 == (e8 * e9))

                e10 = self.pairing.apply(tpk2[0], self.dpk)
                e11 = self.pairing.apply(self.g, ttk2[0])
                e12 = self.pairing.apply(self.h, ttk2[1])
                b4 = (e10 == (e11 * e12))

                e13 = self.pairing.apply(tpk2[1], self.dpk)
                e14 = self.pairing.apply(self.g, ttk2[2])
                e15 = self.pairing.apply(self.h, ttk2[3])
                b5 = (e13 == (e14 * e15))

                e16 = self.pairing.apply(tpk2[2], self.dpk)
                e17 = self.pairing.apply(self.g, ttk2[4])
                e18 = self.pairing.apply(self.h, ttk2[5])
                b6 = (e16 == (e17 * e18))

                b7 = (b1 and b2 and b3 and b4 and b5 and b6)
                if b7:
                    v.append(self.IVgen(tc1[i], tc2[j], ttk1, ttk2))
                else:
                    return None

        pTest_time = time.time() - start

        return v, pTest_time

    def export(self, file):
        with open(file, 'w') as f:
            json.dump(self.info, f, cls=MyEncoder)
