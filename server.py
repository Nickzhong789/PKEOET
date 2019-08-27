import socket
import sys
from pypbc import *
import json


def msg_to_v(m, file):
    with open(file, 'r') as f:
        d_info = json.load(f)
    
    sp = d_info['sp']
    params = Parameters(param_string=sp)
    pairing = Pairing(params)

    dsk_s = d_info['dsk']
    dsk_v = int(dsk_s, 16)
    dsk = Element(pairing, Zr, value=dsk_v)

    v = []
    vt = m.split('.')
    for vti in vt:
        v_s = vti.split('|')
        vi = [Element(pairing, GT, value=v_s[i]) for i in range(len(v_s))]
        v.append(vi)
    
    return v, dsk


def dTest(tv, dsk):
    if((tv[2]**dsk) != tv[3]):
        return None
    else:
        if((tv[0]**dsk) != tv[1]):
            return 0
        else:
            return 1

if __name__ == "__main__":
    json_file = './discriminator.json'


    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    host = socket.gethostname()
    # host = "172.18.92.12"
    port = 8001

    server_socket.bind((host, port))
    server_socket.listen(5)

    print("Server Start...............................")

    while True:
        client_socket, addr = server_socket.accept()

        print("Addr is: %s" % str(addr))

        msg = client_socket.recv(30720)

        # print(msg.decode('utf-8'))

        d_list = []
        v, dsk = msg_to_v(msg.decode('utf-8'), json_file)

        print(v)
        for vi in v:
            d_list.append(dTest(vi, dsk))

        print(d_list)

        client_socket.close()
