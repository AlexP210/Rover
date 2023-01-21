# Echo server program
import socket
import select
import sys
import time
import matplotlib.pyplot as plt
import numpy as np
import time

class RoverClient:
    def __init__(self):
        self.esp = socket.socket()
        self.port = 5000
        self.ip = "192.168.2.28"
        self.esp.connect((self.ip, self.port))
        self.esp.settimeout(0.0)
        self.angles = []
        self.distances = []

    def send(self, command):
        self.esp.send(command.encode())
        time.sleep(0.5)
        try: 
            response = self.esp.recv(1024)
            # print(response)
            return response.decode().strip()
        except BlockingIOError: response = ":BLOCKING ERROR:"
        except UnicodeDecodeError: response= ":DECODE ERROR:"
        return response

    def parse(self, response):
        if "RANGE @" in response:
            angle, distance = map(float, response[8:].split(" = "))
            self.angles.append(angle)
            self.distances.append(distance)
    
    def plot(self):
        plt.scatter(np.array(self.distances)*np.cos(np.array(self.angles)*3.14/180), np.array(self.distances)*np.sin(np.array(self.angles)*3.14/180), c="r")
        plt.plot(np.array(self.distances)*np.cos(np.array(self.angles)*3.14/180), np.array(self.distances)*np.sin(np.array(self.angles)*3.14/180), linewidth=0.5, c="k")
        plt.xlim(left=-4, right=4)
        plt.ylim(bottom=0, top=4)
        plt.show()


        

if __name__ == "__main__":
    client = RoverClient()
    # L = "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.51, 0.51, 0.51, 0.51, 0.51, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.86, 0.69, 2.06, 2.23, 2.23, 2.06, 2.06, 1.89, 2.23, 2.23, 2.92, 2.92, 2.92, 2.06, 2.92, 3.09, 3.09, 3.09, 1.54, 2.92, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.37, 1.54, 1.37, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.54, 1.37, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 2.92, 3.09, 3.43, 3.09, 3.43, 3.43, 3.43, 2.74, 3.43, 3.43, 2.57, 2.57, 2.57, 2.57, 2.74, 2.74, 2.74, 2.57, 2.57, 2.57, 2.57, 2.57, 0.86, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.86, 0.51, 0.69, 0.51, 0.51, 0.69, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.69, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.00, 0.00, 0.00, 0.00"
    # L = "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 1.37, 1.37, 1.37, 1.37, 1.37, 1.37, 1.37, 1.54, 1.37, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.51, 0.51, 0.34, 0.51, 0.51, 0.51, 0.69, 0.34, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.34, 0.51, 0.51, 0.51, 0.69, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.34, 0.51, 0.34, 0.51, 0.51, 0.51, 0.34, 0.51, 0.51, 0.34, 0.51, 0.69, 0.34, 0.51, 0.51, 0.34, 0.51, 0.51, 0.34, 0.51, 0.51, 0.34, 0.51, 0.69, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.34, 0.51, 0.51, 0.51, 0.69, 0.51, 0.51, 0.51, 0.51, 0.51, 0.34, 0.51, 0.51, 0.51, 0.51, 0.51, 0.34, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.51, 0.69, 0.51, 0.69, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.86, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 0.69, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.54, 1.71, 1.54, 1.54, 1.71, 1.54, 1.54, 0.00, 0.00, 0.00, 0.00"
    # values = np.array([float(e) for e in L.split(", ") if e != 0])
    # angles = np.arange(0, 180)
    # plt.plot(values*np.cos(angles*3.14/180), values*np.sin(angles*3.14/180))
    # plt.show()
    while True:
        command = input("> ")
        if "PLOT" in command:
            client.plot()
            command = ""
        if "DRIVE" in command or "SWEEP" in command:
            client.angles = []
            client.distances = []
        responses = client.send(command + "|").split("|")
        for response in responses[:-1]:
            client.parse(response)
            print("Reponse: " + response)