import RPi.GPIO as GPIO
import time
import serial

# Configuracion UART
try:
    ser = serial.Serial("/dev/ttyACM1", 115200, timeout=1)
except Exception as e:
    print(f"Error UART: {e}")
    exit()

# Configuracion Pines HC-SR04
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

TRIG = 18
ECHO = 24

GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

def get_distance():
    GPIO.output(TRIG, False)
    time.sleep(0.05)

    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    pulse_start = time.time()
    pulse_end = time.time()

    while GPIO.input(ECHO) == 0:
        pulse_start = time.time()

    while GPIO.input(ECHO) == 1:
        pulse_end = time.time()

    pulse_duration = pulse_end - pulse_start
    distance = pulse_duration * 17150
    return round(distance, 2)

try:
    print("Iniciando medicion de distancia HC-SR04...")
    while True:
        dist = get_distance()
        print(f"Distancia: {dist} cm")
        
        # Enviar valor numerico a Tiva C via UART
        ser.write(f"{dist}\n".encode('utf-8'))
        
        time.sleep(0.5)
        
except KeyboardInterrupt:
    print("Cerrando programa")
    GPIO.cleanup()
    ser.close()