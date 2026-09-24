# Ejercicio 5 - Raspi 
# PWM Motor DC a duty cycles fijos: 75%, 25%, 45%, 50%

import RPi.GPIO as GPIO
from time import sleep

# Definición de pines (GPIO en formato BCM)
ENA = 12  # Pin 32 físico (PWM Velocidad)
IN1 = 20  # Pin 38 físico (Dirección)
IN2 = 21  # Pin 40 físico (Dirección)

# Configuración inicial de pines
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(ENA, GPIO.OUT)
GPIO.setup(IN1, GPIO.OUT)
GPIO.setup(IN2, GPIO.OUT)

# Configurar sentido de giro del motor
GPIO.output(IN1, GPIO.HIGH)
GPIO.output(IN2, GPIO.LOW)

# Inicializar PWM a 1 kHz
pwm = GPIO.PWM(ENA, 1000)
pwm.start(0)

# Ciclos de trabajo a probar según la guía
duty_cycles = [75, 25, 45, 50]

try:
    print("--- INICIANDO PRUEBA DE MOTOR DC ---")
    for dc in duty_cycles:
        print(f"Estableciendo Duty Cycle al {dc}%...")
        pwm.ChangeDutyCycle(dc)
        sleep(4)  # Mantiene la velocidad durante 4 segundos

    print("Prueba completada. Deteniendo motor...")
    pwm.ChangeDutyCycle(0)

except KeyboardInterrupt:
    print("\nPrueba cancelada por el usuario.")

finally:
    # Apagado seguro y liberación de pines
    pwm.stop()
    GPIO.cleanup()