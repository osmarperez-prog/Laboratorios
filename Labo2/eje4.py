from gpiozero import LED, Button
from time import sleep

# Hardware: 4 LEDs y 2 Botones
led0 = LED(17)
led1 = LED(27)
led2 = LED(22)
led3 = LED(5)

btn_select = Button(6, bounce_time=0.2)
btn_time = Button(13, bounce_time=0.2)

# Funciones individuales para cada LED (Encender solo el LED correspondiente)
def encender_led0():
    led0.on()
    led1.off()
    led2.off()
    led3.off()

def encender_led1():
    led0.off()
    led1.on()
    led2.off()
    led3.off()

def encender_led2():
    led0.off()
    led1.off()
    led2.on()
    led3.off()

def encender_led3():
    led0.off()
    led1.off()
    led2.off()
    led3.on()

# Lista de funciones (Estructura Funcional)
acciones_leds = [encender_led0, encender_led1, encender_led2, encender_led3]

# Variables de estado
indice_led = 0
tiempo_encendido = 1

def apagar_todos():
    led0.off()
    led1.off()
    led2.off()
    led3.off()

print("Ejercicio 4 iniciado.")
print(f"LED actual: {indice_led + 1} | Tiempo programado: {tiempo_encendido} seg")

try:
    while True:
        # Detectar Botón 1: Cambiar de LED y reiniciar tiempo a 1 seg
        if btn_select.is_pressed:
            indice_led = (indice_led + 1) % 4  # Pasa de 0 a 3 y vuelve a 0
            tiempo_encendido = 1
            print(f"\n-> Cambio de LED: LED {indice_led + 1} seleccionado.")
            print(f"-> Tiempo reiniciado a {tiempo_encendido} segundo.")
            
            while btn_select.is_pressed:
                sleep(0.05)

        # Detectar Botón 2: Incrementar tiempo en 1 segundo
        if btn_time.is_pressed:
            tiempo_encendido += 1
            print(f"-> Tiempo aumentado: {tiempo_encendido} segundos.")
            
            while btn_time.is_pressed:
                sleep(0.05)

        # Ejecución funcional del LED actual
        # 1. Obtenemos la función de la lista según el índice
        funcion_led_actual = acciones_leds[indice_led]
        
        # 2. Encendemos el LED mediante su función
        funcion_led_actual()
        
        # 3. Mantenemos encendido durante el tiempo configurado
        sleep(tiempo_encendido)

except KeyboardInterrupt:
    apagar_todos()
    print("\nPrograma finalizado de forma segura.")
