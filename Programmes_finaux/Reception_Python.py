import serial

PORT_COM = 'COM5'
VITESSE = 9600

FICHIER = 'DATA/Mesure.txt'


if __name__ == "__main__":
    ser = serial.Serial(PORT_COM, VITESSE, timeout = 1)
    try:
        while 1:
            line = ser.readline()
            print(line)
            if line: 
                with open(FICHIER,'a') as f:
                    f.write(line.decode())
    except KeyboardInterrupt:
        ser.close()
            
        