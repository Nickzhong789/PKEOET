from pkeet import PKEET
from pypbc import *

import time
import argparse


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

    pkeet = PKEET(pairing, args.num)

    m = pkeet.init_m()

    x1, y1 = pkeet.keygen()
    x2, y2 = pkeet.keygen()
    c1 = pkeet.enc(m, y1)
    c2 = pkeet.enc(m, y2)

    d_list = []
    start = time.time()
    for i in range(len(c1)):
        d_list.append(pkeet.test(c1[i], c2[i]))
    test_time = time.time() - start
    print(test_time)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='PKEET')
    parser.add_argument('--num', type=int, default=10, metavar='N',
                        help='number of chiphertext generated under each pk')

    main(parser.parse_args())
