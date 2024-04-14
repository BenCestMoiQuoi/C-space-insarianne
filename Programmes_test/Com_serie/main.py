import serial

ser = serial.Serial(port= "COM8", baudrate=115200,timeout=1)
ser.close()
ser.open()

if ser.isOpen():
    print(f"{ser.name} is open…")
    print(ser.get_settings())

ser.write(b'hello')

x = ser.read()

s = ser.read(10)

line = ser.readline()