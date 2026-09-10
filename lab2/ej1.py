
import RPi.GPIO as GPIO
import time


LED_1 = 17
LED_2 = 27
BOTON = 5


estado = 1

# Funcines
def cambiar_estado(canal):
    global estado
    estado = estado + 1
    if estado > 4:
        estado = 1

# Configuracion inicial
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(LED_1, GPIO.OUT)
GPIO.setup(LED_2, GPIO.OUT)
GPIO.setup(BOTON, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(BOTON, GPIO.FALLING, callback=cambiar_estado, bouncetime=300)


while True:
    # SALIDAS
    if estado == 1:
        GPIO.output(LED_1, GPIO.HIGH)
        GPIO.output(LED_2, GPIO.LOW)
        time.sleep(1)
        GPIO.output(LED_1, GPIO.LOW)
        GPIO.output(LED_2, GPIO.HIGH)
        time.sleep(1)
    elif estado == 2:
        GPIO.output(LED_1, GPIO.HIGH)
        GPIO.output(LED_2, GPIO.HIGH)
        time.sleep(2)
        GPIO.output(LED_1, GPIO.LOW)
        GPIO.output(LED_2, GPIO.LOW)
        time.sleep(2)
    elif estado == 3:
        GPIO.output(LED_1, GPIO.HIGH)
        GPIO.output(LED_2, GPIO.HIGH)
    elif estado == 4:
        GPIO.output(LED_1, GPIO.LOW)
        GPIO.output(LED_2, GPIO.LOW)