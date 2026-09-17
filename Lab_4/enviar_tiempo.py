import sys
import serial

if len(sys.argv) != 2 or sys.argv[1] not in "123456789" or len(sys.argv[1]) != 1:
    print("Uso: python3 enviar_tiempo.py N")
    print("N debe ser un numero entero de 1 a 9 segundos.")
    sys.exit(1)

dato = sys.argv[1].encode("ascii")

try:
    with serial.Serial(
        "/dev/ttyACM0",
        115200,
        timeout=2,
        write_timeout=2,
        exclusive=True
    ) as puerto:
        puerto.reset_input_buffer()
        puerto.write(dato)
        puerto.flush()

        respuesta = puerto.read(1)

        if respuesta == dato:
            print("Tiva confirmo el intervalo:", sys.argv[1], "segundos.")
        else:
            print("No se recibio la confirmacion de la Tiva.")
            sys.exit(1)

except serial.SerialException as error:
    print("Error de comunicacion:", error)
    sys.exit(1)
