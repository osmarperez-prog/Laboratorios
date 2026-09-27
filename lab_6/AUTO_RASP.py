# LIBRERIAS
import serial
import time

# VARIABLES GLOBALES
PUERTO = "/dev/ttyACM0"
BAUDRATE = 9600

respuestas = {
    "B": "TIVA confirmo: generacion del tono activada.",
    "b": "TIVA confirmo: buzzer apagado.",
    "E": "PARADA ACTIVA: buzzer apagado.",
    "R": "Parada liberada. Buzzer permanece apagado.",
    "!": "Encendido bloqueado por la parada."
}

# CONFIGURACION SERIAL
try:
    puerto_tiva = serial.Serial(
        port=PUERTO,
        baudrate=BAUDRATE,
        timeout=1,
        write_timeout=1,
        exclusive=True
    )
    puerto_tiva.reset_input_buffer()
    time.sleep(1)
    print("Conexion exitosa con la Tiva en " + PUERTO)
except Exception as e:
    print(f"Error al abrir el puerto: {e}")
    exit()

# FUNCIONES
def enviar_comando(cmd):
    # Se unifica el proceso para TODOS los comandos
    mensaje = cmd + "\n"
    puerto_tiva.reset_input_buffer()
    puerto_tiva.write(mensaje.encode('utf-8'))
    print(f"-> Raspberry envia: {cmd}")
    
    # Se lee la linea completa hasta encontrar el salto de linea de la Tiva
    respuesta = puerto_tiva.readline().decode('utf-8').strip()
    
    if respuesta:
        # Si la respuesta corta (E, B, !) está en el diccionario, imprime el texto largo.
        # Si es un texto largo (Motor 1 Alternado), lo imprime tal cual.
        mensaje_amigable = respuestas.get(respuesta, respuesta)
        print(f"<- Tiva responde: {mensaje_amigable}")
    else:
        print("<- Sin respuesta de la Tiva")
    print("-" * 40)

# BUCLE PRINCIPAL
try:
    while True:
        print("\n=== PANEL DE CONTROL INTEGRADO ===")
        print("1: G1 ON         | 2: G1 OFF")
        print("3: G2 ON         | 4: G2 OFF")
        print("5: TODO ON       | 6: TODO OFF")
        print("7: MOTOR 1 (ON/OFF Alternado)")
        print("8: MOTOR 2 (ON/OFF Alternado)")
        print("B: Encender tono (Buzzer)")
        print("b: Apagar tono (Buzzer)")
        print("E: Alternar parada de emergencia")
        print("9: Escribir comando manual libre")
        print("0 / q: Salir")

        op = input("\nOpción: ").strip()

        if op == "1":
            enviar_comando("G1:ON")
        elif op == "2":
            enviar_comando("G1:OFF")
        elif op == "3":
            enviar_comando("G2:ON")
        elif op == "4":
            enviar_comando("G2:OFF")
        elif op == "5":
            enviar_comando("TRASERO:ON")
        elif op == "6":
            enviar_comando("TRASERO:OFF")
        elif op == "7":
            enviar_comando("MOTOR1")
        elif op == "8":
            enviar_comando("MOTOR2")
        elif op == "B":
            enviar_comando("B")
        elif op == "b":
            enviar_comando("b")
        elif op == "E":
            enviar_comando("E")
        elif op == "9":
            cmd_libre = input("Escriba el comando libre: ").strip().upper()
            if cmd_libre:
                enviar_comando(cmd_libre)
        elif op in ("0", "q"):
            break
        else:
            print("Opción no válida. Intente de nuevo.\n")

# MANEJO DE EXCEPCIONES Y CIERRE
except (KeyboardInterrupt, EOFError):
    print("\nSaliendo...")

finally:
    if 'puerto_tiva' in locals() and puerto_tiva.is_open:
        try:
            enviar_comando("b")
        except serial.SerialException:
            pass
        puerto_tiva.close()
        print("Puerto serial cerrado correctamente.")