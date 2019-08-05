import socket
import sys


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

host = socket.gethostname() 
port = 8001

s.connect((host, port))

msg = "hhhhhhhhhhh\r\n"
s.send(msg.encode('utf-8'))
s.close()

print (msg)
