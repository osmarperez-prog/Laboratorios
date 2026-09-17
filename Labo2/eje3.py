from gpiozero import LED, OutputDevice
from time import sleep
import board
import adafruit_dht

# Configuración del sensor DHT22 en GPIO 4
dht_device = adafruit_dht.DHT22(board.D4)

# Dispositivos de control
calentador = LED(17)
ventilador = OutputDevice(27)

print("--- Sistema de Invernadero (Sensor DHT22 Real) ---")

try:
    while True:
        try:
            # Lectura del sensor
            temperatura = dht_device.temperature

            if temperatura is not None:
                print(f"\nTemperatura actual: {temperatura:.1f} °C")

                # Lógica de control
                if temperatura < 12.0:
                    print("Acción: Encendiendo Calentador")
                    calentador.on()
                    ventilador.off()

                elif temperatura > 20.0:
                    print("Acción: Encendiendo Ventilador")
                    calentador.off()
                    ventilador.on()

                else:
                    print("Acción: Estado Óptimo (Apagados)")
                    calentador.off()
                    ventilador.off()

        except RuntimeError as error:
            # Los sensores DHT a veces fallan una lectura puntual, continuamos reintentando
            print(f"Error de lectura temporal: {error.args[0]}")

        sleep(2) # El DHT22 requiere mínimo 2 segundos entre lecturas

except KeyboardInterrupt:
    calentador.off()
    ventilador.off()
    dht_device.exit()
    print("\nSistema detenido de forma segura.")
