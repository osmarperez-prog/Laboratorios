import RPi.GPIO as GPIO
import time


LEDS = [17, 27, 22, 23]
BOTON_SUMAR = 5
BOTON_RESTAR = 6

tabla_binaria = [
    [0, 0, 0, 0],
    [0, 0, 0, 1],
    [0, 0, 1, 0],
    [0, 0, 1, 1],
    [0, 1, 0, 0],
    [0, 1, 0, 1],
    [0, 1, 1, 0],
    [0, 1, 1, 1],
    [1, 0, 0, 0],
    [1, 0, 0, 1],
    [1, 0, 1, 0],
    [1, 0, 1, 1],
    [1, 1, 0, 0],
    [1, 1, 0, 1],
    [1, 1, 1, 0],
    [1, 1, 1, 1]
]

# VARIABLES GLOBALES
contador = 0
estado_anterior_sumar = GPIO.HIGH
estado_anterior_restar = GPIO.HIGH

# CONFIG INICIAL
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
for pin in LEDS:
    GPIO.setup(pin, GPIO.OUT)
GPIO.setup(BOTON_SUMAR, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BOTON_RESTAR, GPIO.IN, pull_up_down=GPIO.PUD_UP)

while True:
    # LECTURA DE ENTRADAS
    estado_sumar = GPIO.input(BOTON_SUMAR)
    estado_restar = GPIO.input(BOTON_RESTAR)

    # LOGICA DE CONTROL
    if estado_sumar == GPIO.LOW and estado_anterior_sumar == GPIO.HIGH:
        contador = contador + 1
        if contador > 15:
            contador = 0
        print("Decimal:", contador, " Hex:", hex(contador), " Binario:", bin(contador))
        time.sleep(0.2)

    if estado_restar == GPIO.LOW and estado_anterior_restar == GPIO.HIGH:
        if contador > 0:
            contador = contador - 1
            print("Decimal:", contador, " Hex:", hex(contador), " Binario:", bin(contador))
        time.sleep(0.2)

    estado_anterior_sumar = estado_sumar
    estado_anterior_restar = estado_restar

    # SALIDAS
    fila = tabla_binaria[contador]
    GPIO.output(LEDS[0], fila[3])
    GPIO.output(LEDS[1], fila[2])
    GPIO.output(LEDS[2], fila[1])
    GPIO.output(LEDS[3], fila[0])