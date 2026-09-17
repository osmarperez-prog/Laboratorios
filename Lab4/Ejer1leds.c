import RPi.GPIO as GPIO
import time
LED1 = 17
LED2 = 27
LED3 = 22

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED1, GPIO.OUT)
GPIO.setup(LED2, GPIO.OUT)
GPIO.setup(LED3, GPIO.OUT)

def leer_tiempo():
    try:
        with open("tiempo.txt", "r") as f:
            return float(f.read())
    except:
        return 1.0  

print("Secuencia iniciada. Modifica 'tiempo.txt' por SSH para cambiar la velocidad.")

try:
    while True:
        
        GPIO.output(LED1, GPIO.HIGH)
        GPIO.output(LED2, GPIO.LOW)
        GPIO.output(LED3, GPIO.LOW)
        time.sleep(leer_tiempo())

        GPIO.output(LED1, GPIO.LOW)
        GPIO.output(LED2, GPIO.HIGH)
        GPIO.output(LED3, GPIO.LOW)
        time.sleep(leer_tiempo())

        
        GPIO.output(LED1, GPIO.LOW)
        GPIO.output(LED2, GPIO.LOW)
        GPIO.output(LED3, GPIO.HIGH)
        time.sleep(leer_tiempo())

except KeyboardInterrupt:
    print("\nPrograma terminado.")
finally:
    GPIO.cleanup()