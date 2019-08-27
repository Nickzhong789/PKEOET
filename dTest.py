from pypbc import *
import json
import time

from utils.logger import Logger
from utils.osutil import *


def dTest(tv, dsk):
    if((tv[2]**dsk) != tv[3]):
        pass
    
    if((tv[0]**dsk) != tv[1]):
        return 0
    else:
        return 1

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
    cusor = 0
    with open('v.txt', 'r') as f:
        start = time.time()

        for l in f.readlines():
            vt = l.strip().split('|')
            v = [Element(pairing, GT, value=vi) for vi in vt]
            e = dTest(v, dsk)

            count += 1
            if count == nums[cusor]:
                print(nums[cusor])
                t = time.time() - start
                logger.append([count, t])
                cusor += 1
                print('T: ', t)
                print(cusor)
