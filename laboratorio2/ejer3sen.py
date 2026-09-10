import time
import board
import digitalio
import adafruit_dht

led_calefactor = digitalio.DigitalInOut(board.D17)
led_calefactor.direction = digitalio.Direction.OUTPUT

led_ventilador = digitalio.DigitalInOut(board.D27)
led_ventilador.direction = digitalio.Direction.OUTPUT

# CONFIGURACION DEL SENSOR
sensor = adafruit_dht.DHT11(board.D4, use_pulseio=False)

while True:
    try:
        temperatura = sensor.temperature
        print("Temperatura actual:", temperatura, "C")

        if temperatura < 12:
            led_calefactor.value = True
            led_ventilador.value = False
        elif temperatura > 20:
            led_calefactor.value = False
            led_ventilador.value = True
        else:
            led_calefactor.value = False
            led_ventilador.value = False

    except RuntimeError:
        pass

    time.sleep(2)