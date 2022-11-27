import time
import serial

class Measurement:
    def __init__(self, timestamp, data_dictionary):
        self.time = timestamp
        self.data = data_dictionary
    def __getitem__(self, keys):
        return Measurement(
            time.time(),
            {key: self.data[key.lower()] for key in keys}
        )

class Sensor:
    def __init__(self, port):
        self.serial_port = serial.Serial(port, 9600, timeout=1)
        if not self.serial_port.is_open: self.serial_port.open()

        self.measurement_types = (
            "temperature",
            "humidity",
            "light",
        )
        self.last_measurement = Measurement(
            time.time(),
            {measurement_type: ("0", "x") for measurement_type in self.measurement_types},
        )

    def close(self):
        self.serial_port.close()

    def poll(self):
        data = {}
        while not all([measurement_type in data for measurement_type in self.measurement_types]):
            readings = self.serial_port.read_until(b'\n').decode("ascii").strip().split(", ")
            print(readings)
            for reading in readings:
                print("     ", reading)
                measurement_type, value_and_unit = reading.split(": ")
                if measurement_type in self.measurement_types: data[measurement_type] = value_and_unit
        self.last_measurement = Measurement(time.time(), data)
        return self.last_measurement

    def get_values(self, search_strings):
        measurement = self.poll()
        return measurement[search_strings]
