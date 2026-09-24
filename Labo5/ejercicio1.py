import serial
import time

try:
    ser = serial.Serial('/dev/ttyACM1', 9600, timeout=1)
    time.sleep(2)
    
    while True:
        led = input("Seleccione LED (PN1, PN0, PF4, PF0): ")
        state = input("Accion (ON / OFF): ")
        comando = f"{led}:{state}\n"
        ser.write(comando.encode('utf-8'))
        
        respuesta = ser.readline().decode('utf-8').strip()
        print(f"Respuesta TIVA: {respuesta}")
except Exception as e:
    print(f"Error UART: {e}")