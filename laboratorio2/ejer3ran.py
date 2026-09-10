import RPi.GPIO as GPIO
import time
import random

LED_CALEFACTOR = 17
LED_VENTILADOR = 27

# CONFIGURACION INICIAL
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(LED_CALEFACTOR, GPIO.OUT)
GPIO.setup(LED_VENTILADOR, GPIO.OUT)

while True:
    temperatura = random.randint(5, 30)
    print("Temperatura generada:", temperatura, "C")

    if temperatura < 12:
        GPIO.output(LED_CALEFACTOR, GPIO.HIGH)
        GPIO.output(LED_VENTILADOR, GPIO.LOW)
    elif temperatura > 20:
        GPIO.output(LED_CALEFACTOR, GPIO.LOW)
        GPIO.output(LED_VENTILADOR, GPIO.HIGH)
    else:
        GPIO.output(LED_CALEFACTOR, GPIO.LOW)
        GPIO.output(LED_VENTILADOR, GPIO.LOW)

    time.sleep(2)