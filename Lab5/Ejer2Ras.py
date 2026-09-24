import serial
import time

try:
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    print("Conexión establecida")
    
    while True:
        seg = input("\nIngresar los segundos(1-9): ")
        if seg.lower() == 'x':
            break
        if seg.isdigit() and 1 <= int(seg) <= 9:
            ser.write(seg.encode('utf-8'))
            print(f"-> Enviado")
        else:
            print("No válido")

except Exception as e:
    print(f"Error al conectar")