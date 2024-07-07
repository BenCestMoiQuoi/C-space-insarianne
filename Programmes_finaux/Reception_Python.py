import serial

PORT_COM = 'COM6'
VITESSE = 9600

class Serie():
    port: str
    baudrate: int
    ser: serial.Serial
    
    def __init__(self, port: str, baudrate: int = 9600) -> None:
        self.port = port
        self.baudrate = baudrate
        self.ser = serial.Serial(self.port, self.baudrate)
    
    def close(self) -> None:
        self.ser.close()

    def write(self, txt: str) -> None:
        self.ser.write(txt.encode())

    def read(self, state: int = 1) -> str:
        return self.ser.read(state)

if __name__ == "__main__":
    ser = serial.Serial('COM6', 115200, timeout = 1)
    try:
        while 1:
            line = ser.readline()
            print(line)
            if line: 
                with open('monfichier.txt','a') as f:
                    f.write(line.decode())
    except KeyboardInterrupt:
        f.close()
        ser.close()
            
        