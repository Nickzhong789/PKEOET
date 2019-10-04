from pypbc import *
import time


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

g = Element.random(pairing, G1)

st = time.time()
with open('./ciphertexts/c_gt.txt', 'w') as f:
    for i in range(100):
        t_m = Element.random(pairing, G2)
        m = pairing.apply(g, t_m)
        f.write(str(m) + '\n')

gen_time = time.time() - st
print("Gen Time: ", gen_time)
