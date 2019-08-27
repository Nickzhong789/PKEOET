from pkeoet import PKEOET
from pypbc import *
import argparse


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


def list_to_str(l):
    s = ''
    for e in l:
        s += str(e) + '|'
    
    return s[:-1]


def main(args):
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

    pkeoet = PKEOET(pairing, args.num)
    print(pkeoet.pairing == pairing)

    pkeoet.setup()

    m_a = pkeoet.init()

    pkeoet.dkg()

    sk1, pk1 = pkeoet.ukg()
    tk1 = pkeoet.tkg(sk1)

    user1 = {}
    # user1['sk'] = list_to_str(sk1)
    user1['pk'] = list_to_str(pk1)
    user1['tk'] = list_to_str(tk1)

    sk2, pk2 = pkeoet.ukg()
    tk2 = pkeoet.tkg(sk2)

    user2 = {}
    # user2['sk'] = list_to_str(sk1)
    user2['pk'] = list_to_str(pk1)
    user2['tk'] = list_to_str(tk1)

    c1 = pkeoet.enc(pk1, tk1, sk1, m_a)
    c2 = pkeoet.enc(pk2, tk2, sk2, m_a)
    c = c1 + c2

    with open('./ciphertext.txt', 'w') as f:
        for ci in c:
            s = ''
            for e in ci:
                s += str(e) + '|'
            f.write(s[:-1])
            f.write('\n')
    
    # v = pkeoet.pTest(c1, c2, pk1, pk2, tk1, tk2)

    pkeoet.info['sp'] = stored_params
    pkeoet.info['dpk'] = str(pkeoet.dpk)
    pkeoet.info['dsk'] = str(pkeoet.alpha)

    file1 = './discriminator.json'
    pkeoet.export(file1)

    pkeoet.info['user1'] = user1
    pkeoet.info['user2'] = user2
    pkeoet.info['g'] = str(pkeoet.g)
    pkeoet.info['h'] = str(pkeoet.h)

    file2 = './cli.json'
    pkeoet.export(file2)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='PKEOET')
    parser.add_argument('--num', type=int, default=10, metavar='N',
                        help='number of chiphertext generated under each pk')

    main(parser.parse_args())
