from pypbc import *
import hashlib
import time


class PKEET(object):
    def __init__(self, pairing, num):
        super(PKEET, self).__init__()
        self.pairing = pairing

        self.num = num
        self.g = Element(self.pairing, G1)

    def setup(self):
        self.g = Element.random(self.pairing, G1)

    def init_m(self):
        m = []
        for i in range(self.num):
            m.append(Element.random(self.pairing, G1))

        return m

    def keygen(self):
        x = Element.random(self.pairing, Zr)
        y = self.g**x

        return x, y

    def get_hash(self, c1, c2, c3):
        h_in = str(c1) + str(c2) + str(c3)
        sha512 = hashlib.sha512()
        sha512.update(h_in.encode('utf8'))
        hv = sha512.hexdigest()
        ev = Element.from_hash(self.pairing, G1, hv)

        return ev

    def enc(self, m, y):
        c = []
        for i in range(self.num):
            r = Element.random(self.pairing, Zr)
            u = self.g**r
            v = m[i]**r

            h = self.get_hash(u, v, y**r)
            w = h
            c.append([u, v, w])
        
        return c

    def test(self, c1, c2):
        if self.pairing.apply(c1[0], c2[1]) == self.pairing.apply(c2[0], c1[1]):
            return 1
        else:
            return 0
