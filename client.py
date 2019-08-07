import socket
import sys

from pkeoet import PKEOET


def IVgen(num):
    stored_params = """type a
    q 8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791
    h 12016012264891146079388821366740534204802954401251311822919615131047207289359704531102844802183906537786776
    r 730750818665451621361119245571504901405976559617
    exp2 159
    exp1 107
    sign1 1
    sign0 1
    """

    pkeoet = PKEOET(stored_params, num)

    pkeoet.setup()

    m_a = pkeoet.init()

    pkeoet.dkg()

    sk1, pk1 = pkeoet.ukg()
    tk1 = pkeoet.tkg(sk1)

    sk2, pk2 = pkeoet.ukg()
    tk2 = pkeoet.tkg(sk2)

    c1 = pkeoet.enc(pk1, tk1, sk1, m_a)
    c2 = pkeoet.enc(pk2, tk2, sk2, m_a)

    v = pkeoet.pTest(c1, c2, pk1, pk2, tk1, tk2)

    return v

v = IVgen(10)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

host = socket.gethostname() 
port = 8001

s.connect((host, port))

msg = "hhhhhhhhhhh\r\n"
s.send(msg.encode('utf-8'))
s.close()

print (msg)
