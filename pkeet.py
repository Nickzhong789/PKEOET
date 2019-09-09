from pypbc import *
import hashlib
import time


class PKEET(object):
    def __init__(self, pairing):
        super(PKEET, self).__init__()
        self.pairing = pairing

        self.g = Element(self.pairing, G1)

    def setup(self):
        self.g = Element.random(self.pairing, G1)

    def init_m(self, num):
        m = []
        for i in range(num):
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

        return hv

    def enc(self, m, y):
        start = time.time()
        r = Element.random(self.pairing, Zr)
        u = self.g**r
        v = m**r
        h = self.get_hash(u, v, y**r)
        mr_s = str(m) + str(r)

        w = ''
        for i in range(len(h)):
            w += chr(ord(h[i]) ^ ord(mr_s[i])) 
        c = [str(u), str(v), w]
        enc_time = time.time() - start
        
        return c, enc_time

    def test(self, c1, c2):
        u1 = Element(self.pairing, G1, value=c1[0])
        u2 = Element(self.pairing, G1, value=c2[0])
        v1 = Element(self.pairing, G1, value=c1[1])
        v2 = Element(self.pairing, G1, value=c2[1])

        start = time.time()

        if  self.pairing.apply(u1, v2) == self.pairing.apply(u2, v1):
            ret = 1
        else:
            ret = 0

        end = time.time() - start

        return ret, end
