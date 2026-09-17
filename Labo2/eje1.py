#!/usr/bin/env python3
from gpiozero import LED, Button
from time import sleep

# Configuración de pines
led1 = LED(17)
led2 = LED(27)
button = Button(22, bounce_time=0.2)

# 1. Definimos cada estado como una función sencilla
def estado_1():
    print("Estado 1: Alternando LEDs (1s)")
    led1.on()
    led2.off()
    sleep(1)
    led1.off()
    led2.on()
    sleep(1)

def estado_2():
    print("Estado 2: Parpadeo simultáneo (2s)")
    led1.on()
    led2.on()
    sleep(2)
    led1.off()
    led2.off()
    sleep(2)

def estado_3():
    print("Estado 3: LEDs encendidos")
    led1.on()
    led2.on()
    sleep(0.1)

def estado_4():
    print("Estado 4: LEDs apagados")
    led1.off()
    led2.off()
    sleep(0.1)

# 2. Guardamos las funciones dentro de una lista (Programación Funcional básica)
lista_estados = [estado_1, estado_2, estado_3, estado_4]
posicion = 0  # Empezamos en la posición 0 (Estado 1)

print("Ejercicio 1 iniciado. Presiona el botón para avanzar de estado.")

try:
    while True:
        # Detectar el botón y cambiar de posición en la lista
        if button.is_pressed:
            posicion = posicion + 1
            
            # Si pasamos del Estado 4 (posición 3), volvemos al Estado 1 (posición 0)
            if posicion > 3:
                posicion = 0
                
            # Esperar a que se suelte el botón
            while button.is_pressed:
                sleep(0.05)

        # 3. Llamamos a la función correspondiente según la posición actual
        funcion_actual = lista_estados[posicion]
        funcion_actual()

except KeyboardInterrupt:
    led1.off()
    led2.off()
    print("\nPrograma detenido de forma segura.")
