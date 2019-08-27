import socket
import sys
import json

from pkeoet import PKEOET
from pypbc import *


def list_to_str(l):
    s = ''
    for e in l:
        s += str(e) + '|'
    
    return s[:-1]


def str_to_c(file, pairing):
    with open(file, 'r') as f:
        ct = f.read().splitlines()

    seg = len(ct) // 2

    c = []
    for ci in ct:
        ci_list = ci.split('|')

        ci1 = Element(pairing, G1, value=ci_list[0])
        ci2 = Element(pairing, G1, value=ci_list[1])
        ci3 = Element(pairing, GT, value=ci_list[2])
        ci4 = Element(pairing, GT, value=ci_list[3])

        c.append([ci1, ci2, ci3, ci4])

    c1 = c[:seg]
    c2 = c[seg:]

    return c1, c2


def IVgen(num, json_file):
    with open(json_file, 'r') as f:
        server_info = json.load(f)

    stored_params = server_info['sp']
    params = Parameters(param_string=stored_params)
    pairing = Pairing(params)

    pkeoet = PKEOET(pairing, num)
    # print(pairing == pkeoet.pairing)

    pk1 = [Element(pairing, G1, value=pki) for pki in server_info['user1']['pk'].split('|')]
    tk1 = [Element(pairing, G2, value=tki) for tki in server_info['user1']['tk'].split('|')]

    pk2 = [Element(pairing, G1, value=pki) for pki in server_info['user2']['pk'].split('|')]
    tk2 = [Element(pairing, G2, value=tki) for tki in server_info['user2']['tk'].split('|')]

    c_file = './ciphertext.txt'
    c1, c2 = str_to_c(c_file, pairing)

    dpk = Element(pairing, G2, value=server_info['dpk'])
    g = Element(pairing, G1, value=server_info['g'])
    h = Element(pairing, G1, value=server_info['h'])

    v = pkeoet.pTest(c1, c2, pk1, pk2, tk1, tk2)

    return v


if __name__ == "__main__":
    json_file = './cli.json'
    v = IVgen(3, json_file)
    print(v[0])

    # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

    # host = socket.gethostname() 
    # port = 8001

    # s.connect((host, port))

    with open('./v.txt', 'w') as f:
        msg = ''
        for vi in v:
            m = list_to_str(vi)
            f.write(m)
            f.write('\n')
            msg += m + '.'
    
    
    # s.send(msg[:-1].encode('utf-8'))
    # print (msg)
    
    # s.close()
