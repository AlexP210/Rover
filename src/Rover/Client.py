# Echo server program
import socket
import select
import sys
import time

class RoverClient:
    def __init__(self):
        self.esp = socket.socket()
        self.port = 5000
        self.ip = "192.168.2.28"
        self.esp.connect((self.ip, self.port))
        self.esp.setblocking(0)

    def send(self, command):
        try: self.esp.send(command.encode())
        except: pass
        time.sleep(1)
        try: return self.esp.recv(1024).decode().strip()
        except BlockingIOError: return ":BLOCKING ERROR:"
        except UnicodeDecodeError: return ":DECODE ERROR:"
    
if __name__ == "__main__":
    client = RoverClient()
    while True:
        response = client.send(input("Enter Command: "))
        print("Reponse: " + response)