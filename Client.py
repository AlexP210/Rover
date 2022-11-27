# Echo server program
import socket
import select
import sys
import time

esp = socket.socket()

port = 5000
ip = "169.254.158.250"
# ip = "192.168.2.28"

# ip = "192.168.1.13"

esp.connect((ip, port))
esp.setblocking(0)

while True:
    try: data = print(esp.recv(1024).decode().strip())
    except BlockingIOError: pass
    msg = input("Enter message: ") + "\n"
    try: esp.send(msg.encode())
    except: pass
    time.sleep(1)
