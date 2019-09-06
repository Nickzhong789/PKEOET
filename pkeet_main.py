from pkeet import PKEET
from pypbc import *
from utils.logger import Logger
from utils.osutil import *

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

    pkeet = PKEET(pairing)

    pkeet.setup()

    x1, y1 = pkeet.keygen()
    x2, y2 = pkeet.keygen()

    logger = Logger(join('./output', 'pkeet_log.txt'), title='PKEET')
    logger.set_names(['C Num', 'Enc Time', 'Test Time'])

    nums1 = [10, 20, 30, 40, 50, 100, 150, 200]
    nums2 = [i*100 for i in range(3, 901)]
    nums = nums1 + nums2
    print(nums[-1])

    cursor = 0
    count = 0
    enc_time = 0
    test_time = 0
    d_list = []

    with open('./ciphertexts/c.txt', 'r') as f:
        for l in f.readlines():
            count += 1
            m = Element(pairing, G1, value=l.strip())

            start = time.time()
            c1 = pkeet.enc(m, y1)
            enc_time += time.time() - start
            c2 = pkeet.enc(m, y2)

            t_ret, t_t = pkeet.test(c1, c2)
            d_list.append(t_ret)
            test_time += t_t
            print(t_t)

            if count == nums[cursor]:
                print('Enc %d Time: %s' % (count, enc_time))
                print('Test %d Time: %s' % (count, test_time))
                logger.append([count, enc_time, test_time])
                cursor += 1


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='PKEET')
    parser.add_argument('--num', type=int, default=10, metavar='N',
                        help='number of chiphertext generated under each pk')

    main(parser.parse_args())
