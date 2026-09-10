import RPi.GPIO as GPIO
import time

LEDS = [17, 27, 22, 23]
BOTON_SELECCION = 5
BOTON_TIEMPO = 6

led_actual = 0
tiempo_encendido = 1

# FUNCIONES
def cambiar_led(canal):
    global led_actual
    global tiempo_encendido
    led_actual = led_actual + 1
    if led_actual > 3:
        led_actual = 0
    tiempo_encendido = 1

def aumentar_tiempo(canal):
    global tiempo_encendido
    tiempo_encendido = tiempo_encendido + 1


# CONFIGURACION INICIAL
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
for pin in LEDS:
    GPIO.setup(pin, GPIO.OUT)
GPIO.setup(BOTON_SELECCION, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BOTON_TIEMPO, GPIO.IN, pull_up_down=GPIO.PUD_UP)

GPIO.add_event_detect(BOTON_SELECCION, GPIO.FALLING, callback=cambiar_led, bouncetime=300)
GPIO.add_event_detect(BOTON_TIEMPO, GPIO.FALLING, callback=aumentar_tiempo, bouncetime=300)

while True:
    GPIO.output(LEDS[led_actual], GPIO.HIGH)
    time.sleep(tiempo_encendido)
    GPIO.output(LEDS[led_actual], GPIO.LOW)
    time.sleep(0.5)