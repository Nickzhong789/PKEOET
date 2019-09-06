from pypbc import *
import json
import time

from utils.logger import Logger
from utils.osutil import *


def dTest(tv, dsk):
    start = time.time()
    if((tv[2]**dsk) != tv[3]):
        pass
    
    if((tv[0]**dsk) != tv[1]):
        ret = 0
    else:
        ret = 1
    
    end = time.time() - start

    return ret, end

if __name__ == '__main__':
    with open('./discriminator.json', 'r') as f:
        d_info = json.load(f)
    
    sp = d_info['sp']
    params = Parameters(param_string=sp)
    pairing = Pairing(params)

    dsk_s = d_info['dsk']
    dsk_v = int(dsk_s, 16)
    dsk = Element(pairing, Zr, value=dsk_v)

    logger = Logger(join('./output', 'log_d.txt'), title="PKEOET")
    logger.set_names(['V Num', 'DTest Time'])

    nums1 = [10, 20, 30, 40, 50, 100, 150, 200]
    nums2 = [i*100 for i in range(3, 1000)]
    nums = nums1 + nums2

    count = 0
    cursor = 0
    dTest_time = 0
    with open('./ciphertexts/v.txt', 'r') as f:
        for l in f.readlines():
            count += 1
            vt = l.strip().split('|')
            v = [Element(pairing, GT, value=vi) for vi in vt]
            d_ret, d_t = dTest(v, dsk)
            dTest_time += d_t

            if count == nums[cursor]:
                cursor += 1
                logger.append([count, dTest_time])
                print('dTest %s Time: %s' % (count, dTest_time))
