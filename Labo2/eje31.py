ifrom gpiozero import LED, OutputDevice
from time import sleep
import random

# Hardware: LED Rojo para calentador, OutputDevice/LED para ventilador
calentador = LED(17)
ventilador = OutputDevice(27)

print("--- Sistema de Invernadero (Modo Simulación) ---")

try:
    while True:
        # Generar temperatura aleatoria entre 5 y 30 °C
        temperatura = round(random.uniform(5.0, 30.0), 1)
        print(f"\nTemperatura registrada: {temperatura} °C")

        # Reglas de control
        if temperatura < 12.0:
            print("Estado: FÉRREO FRÍO (< 12°C) -> Encendiendo Calentador")
            calentador.on()
            ventilador.off()

        elif temperatura > 20.0:
            print("Estado: EXCESO DE CALOR (> 20°C) -> Encendiendo Ventilador")
            calentador.off()
            ventilador.on()

        else:
            print("Estado: OPTIMAL (12°C - 20°C) -> Apagando ambos dispositivos")
            calentador.off()
            ventilador.off()

        sleep(2)

except KeyboardInterrupt:
    calentador.off()
    ventilador.off()
    print("\nSistema detenido.")
