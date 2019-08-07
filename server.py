import socket
import sys


server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# host = socket.gethostname()
host = "172.18.92.12"
port = 8001

server_socket.bind((host, port))
server_socket.listen(5)

while True:
    client_socket, addr = server_socket.accept()

    print("Addr is: %s" % str(addr))
    
    msg=client_socket.recv(10240)

    print(msg.decode('utf-8'))
    
    client_socket.close()
