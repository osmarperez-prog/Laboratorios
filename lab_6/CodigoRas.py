import socket
import serial
import time
import threading
import subprocess

# MANDO PS4
try:
    import evdev
    from evdev import ecodes
except ImportError:
    evdev = None

# CONFIGURACIÓN PUERTO SERIAL
PUERTO_SERIAL = "/dev/ttyACM0"
BAUDRATE = 9600

# CONFIGURACIÓN GENERAL
CANAL_RFCOMM = 1
VELOCIDAD_INICIAL = 70
PASO_VELOCIDAD = 10
ZONA_MUERTA = 0.12

try:
    puerto_tiva = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=BAUDRATE,
        timeout=1,
        write_timeout=1,
        exclusive=False
    )
    puerto_tiva.reset_input_buffer()
    time.sleep(1)
    print("Conexión serial exitosa con la Tiva C.")
except Exception as e:
    print(f"Error al abrir puerto serial con la Tiva: {e}")
    exit()

# ESTADO DEL ROBOT
estado = {
    "motor1": False,
    "motor2": False,
    "luces": False,
    "parqueo": False,
    "buzzer": False,
    "parada": False,
    "modo": "GUI",
    "velocidad": VELOCIDAD_INICIAL,
    "conduccion": [0, 0],
}
candado = threading.RLock()
corriendo = True

def imprimir_estado():
    on = lambda v: "ENCENDIDO" if v else "apagado"
    e = estado
    izq, der = e["conduccion"]
    print(f"   [ESTADO] Motor 1 (izq): {on(e['motor1'])} | Motor 2 (der): {on(e['motor2'])} | "
          f"Luces traseras: {on(e['luces'])} | Parqueo: {'PARPADEANDO' if e['parqueo'] else 'apagado'} | "
          f"Buzzer: {on(e['buzzer'])}")
    print(f"            Velocidad: {e['velocidad']}% | Modo: {e['modo']} | "
          f"Parada de emergencia: {'ACTIVA' if e['parada'] else 'no'} | "
          f"Joystick: izq={izq:+d}% der={der:+d}%")

def linea_estado():
    e = estado
    return ("STATE:M1={:d},M2={:d},LED={:d},PARK={:d},BUZ={:d},E={:d},SPD={},MODE={},DRV={},{}"
            .format(e["motor1"], e["motor2"], e["luces"], e["parqueo"], e["buzzer"],
                    e["parada"], e["velocidad"], e["modo"], *e["conduccion"]))

# FUNCIÓN DE TRADUCCIÓN
def traducir_comando(cmd_app):
    mapeo = {
        "M1:1": "MOTOR1",
        "M1:0": "MOTOR1",
        "M2:1": "MOTOR2",
        "M2:0": "MOTOR2",
        "LED:1": "TRASERO:ON",
        "LED:0": "TRASERO:OFF",
        "BUZ:1": "B",
        "BUZ:0": "b",
        "ESTOP": "E",
        "PARK:1": "PARK:1",
        "PARK:0": "PARK:0",
    }

    if cmd_app in mapeo:
        return mapeo[cmd_app]

    if cmd_app.startswith("SPD:"):
        valor_velocidad = cmd_app.split(":")[1]
        return f"VEL:{valor_velocidad}"

    if cmd_app in ["MODE:PS5", "MODE:PS4", "MODE:MANUAL", "MODE:GUI", "GET"]:
        return None

    return cmd_app

def enviar_a_tiva(cmd_tiva, silencioso=False):
    if not cmd_tiva:
        return

    mensaje = cmd_tiva + "\n"
    with candado:
        puerto_tiva.write(mensaje.encode('utf-8'))
    if not silencioso:
        print(f"-> Reenviado a Tiva C: {cmd_tiva}")

def leer_tiva():
    while corriendo:
        try:
            respuesta = puerto_tiva.readline().decode('utf-8', 'replace').strip()
        except Exception as e:
            print(f"Error leyendo la Tiva: {e}")
            time.sleep(1)
            continue
        if respuesta:
            print(f"<- Tiva responde: {respuesta}")

# LÓGICA
def _poner(clave, cmd_on, cmd_off, quiero):
    if estado[clave] != quiero:
        enviar_a_tiva(traducir_comando(cmd_on if quiero else cmd_off))
        estado[clave] = quiero

def _detener_conduccion():
    if estado["conduccion"] != [0, 0]:
        estado["conduccion"] = [0, 0]
        enviar_a_tiva("DRV:0,0")

def procesar_comando(cmd_app, origen="app"):
    cmd = cmd_app if cmd_app in ("B", "b") else cmd_app.upper()
    if cmd == "B":
        cmd = "BUZ:1"
    elif cmd == "b":
        cmd = "BUZ:0"

    with candado:
        if cmd == "GET":
            return linea_estado()

        if cmd.startswith("MODE:"):
            modo = "GUI" if cmd in ("MODE:GUI", "MODE:APP") else "MANUAL"
            if estado["modo"] != modo:
                estado["modo"] = modo
                _detener_conduccion()
                if not estado["parada"]:
                    _poner("buzzer", "BUZ:1", "BUZ:0", False)
                print(f"   MODO cambiado a {modo} "
                      f"({'manda la App' if modo == 'GUI' else 'manda el mando PS4'})")
                imprimir_estado()
            return f"OK:MODE={modo}"

        if cmd.startswith("SPD:"):
            valor = cmd.split(":", 1)[1]
            try:
                nueva = estado["velocidad"] + int(valor) if valor[:1] in "+-" else int(float(valor))
            except ValueError:
                return "ERR:VELOCIDAD_INVALIDA"
            nueva = max(0, min(100, nueva))
            estado["velocidad"] = nueva
            enviar_a_tiva(traducir_comando(f"SPD:{nueva}"))
            imprimir_estado()
            return f"OK:SPD={nueva}"

        if cmd in ("ESTOP", "E"):
            if not estado["parada"]:
                _detener_conduccion()
                _poner("motor1", "M1:1", "M1:0", False)
                _poner("motor2", "M2:1", "M2:0", False)
                _poner("luces", "LED:1", "LED:0", False)
                _poner("parqueo", "PARK:1", "PARK:0", False)
                _poner("buzzer", "BUZ:1", "BUZ:0", False)
            enviar_a_tiva(traducir_comando("ESTOP"))
            estado["parada"] = not estado["parada"]
            print(f"   PARADA DE EMERGENCIA {'ACTIVADA' if estado['parada'] else 'LIBERADA'}")
            imprimir_estado()
            return f"OK:ESTOP={int(estado['parada'])}"

        if origen == "app" and estado["modo"] != "GUI":
            print(f"   Rechazado: '{cmd}' (modo MANUAL, manda el mando PS4)")
            return "ERR:MODO_MANUAL_ACTIVO"
        if origen == "mando" and estado["modo"] != "MANUAL":
            return "ERR:MODO_GUI_ACTIVO"
        if estado["parada"]:
            print(f"   Rechazado: '{cmd}' (parada de emergencia activa)")
            return "ERR:PARADA_ACTIVA"

        if cmd.startswith("DRV:"):
            try:
                izq, der = (max(-100, min(100, int(float(v)))) for v in cmd[4:].split(","))
            except ValueError:
                return "ERR:DRV_INVALIDO"
            antes = estado["conduccion"]
            estado["conduccion"] = [izq, der]
            if izq or der:
                estado["motor1"] = estado["motor2"] = False
            enviar_a_tiva(f"DRV:{izq},{der}", silencioso=True)
            if (izq or der) and antes == [0, 0]:
                print("   Conducción con joystick iniciada")
                imprimir_estado()
            elif not (izq or der) and antes != [0, 0]:
                print("   Conducción con joystick detenida")
                imprimir_estado()
            return f"OK:DRV={izq},{der}"

        alternar = {"MOTOR1": ("motor1", "M1"), "MOTOR2": ("motor2", "M2"),
                    "LED": ("luces", "LED"), "PARK": ("parqueo", "PARK"),
                    "BUZ": ("buzzer", "BUZ")}
        if cmd in alternar:
            clave, base = alternar[cmd]
            cmd = f"{base}:{0 if estado[clave] else 1}"

        if cmd in ("M1:1", "M1:0", "M2:1", "M2:0"):
            clave = "motor1" if cmd.startswith("M1") else "motor2"
            _detener_conduccion()
            _poner(clave, cmd[:3] + "1", cmd[:3] + "0", cmd.endswith("1"))
            imprimir_estado()
            return f"OK:{cmd[:2]}={int(estado[clave])}"

        if cmd in ("LED:1", "LED:0"):
            _poner("luces", "LED:1", "LED:0", cmd.endswith("1"))
            imprimir_estado()
            return f"OK:LED={int(estado['luces'])}"

        if cmd in ("PARK:1", "PARK:0"):
            _poner("parqueo", "PARK:1", "PARK:0", cmd.endswith("1"))
            imprimir_estado()
            return f"OK:PARK={int(estado['parqueo'])}"

        if cmd in ("BUZ:1", "BUZ:0"):
            _poner("buzzer", "BUZ:1", "BUZ:0", cmd.endswith("1"))
            imprimir_estado()
            return f"OK:BUZ={int(estado['buzzer'])}"

        if cmd == "STOP":
            _detener_conduccion()
            _poner("motor1", "M1:1", "M1:0", False)
            _poner("motor2", "M2:1", "M2:0", False)
            imprimir_estado()
            return "OK:STOP"

        cmd_tiva = traducir_comando(cmd_app)
        if cmd_tiva:
            enviar_a_tiva(cmd_tiva)
            return f"OK:{cmd_tiva}"
        return "OK"

def mantener_conduccion():
    while corriendo:
        time.sleep(0.15)
        with candado:
            izq, der = estado["conduccion"]
            if (izq or der) and not estado["parada"]:
                enviar_a_tiva(f"DRV:{izq},{der}", silencioso=True)

def mezcla(x, y):
    mag = min(1.0, (x * x + y * y) ** 0.5)
    if mag < ZONA_MUERTA:
        return 0, 0
    k = (mag - ZONA_MUERTA) / (1 - ZONA_MUERTA) / mag
    x, y = x * k, y * k
    izq, der = y + x, y - x
    m = max(1.0, abs(izq), abs(der))
    return round(izq / m * 100), round(der / m * 100)

def buscar_mando():
    for ruta in evdev.list_devices():
        try:
            dev = evdev.InputDevice(ruta)
        except OSError:
            continue
        caps = dev.capabilities()
        teclas = caps.get(ecodes.EV_KEY, [])
        ejes = [a[0] if isinstance(a, tuple) else a for a in caps.get(ecodes.EV_ABS, [])]
        nombre = dev.name.lower()
        if ("motion" in nombre or "touchpad" in nombre or
                ecodes.BTN_SOUTH not in teclas or ecodes.ABS_X not in ejes):
            dev.close()
            continue
        return dev
    return None

def hilo_mando():
    if not evdev:
        print("python3-evdev no instalado: mando PS4 deshabilitado")
        return
    botones = {ecodes.BTN_SOUTH: "x", ecodes.BTN_EAST: "circulo",
               ecodes.BTN_NORTH: "triangulo", ecodes.BTN_WEST: "cuadrado",
               ecodes.BTN_TL: "l1", ecodes.BTN_TR: "r1",
               ecodes.BTN_START: "options", ecodes.BTN_MODE: "ps"}
    nombres_ejes = {ecodes.ABS_X: "lx", ecodes.ABS_Y: "ly",
                    ecodes.ABS_RX: "rx", ecodes.ABS_RY: "ry"}
    acciones = {"triangulo": "PARK", "circulo": "LED", "cuadrado": "STOP",
                "l1": "MOTOR1", "r1": "MOTOR2"}
    aviso = True
    while corriendo:
        dev = buscar_mando()
        if not dev:
            if aviso:
                print("Esperando mando PS4 (presiona el botón PS)...")
                aviso = False
            time.sleep(1.5)
            continue
        rangos = {}
        for code in nombres_ejes:
            try:
                info = dev.absinfo(code)
                rangos[code] = (info.min, info.max)
            except Exception:
                pass
        ejes = {"lx": 0.0, "ly": 0.0, "rx": 0.0, "ry": 0.0}
        ultimo = (0, 0)
        print(f"\n¡Mando PS4 conectado!: {dev.name}")
        try:
            for ev in dev.read_loop():
                if ev.type == ecodes.EV_KEY and ev.code in botones and ev.value != 2:
                    nombre, presionado = botones[ev.code], ev.value == 1
                    if presionado or nombre == "x":
                        print(f"\n<- Mando: botón {nombre.upper()} "
                              f"{'presionado' if presionado else 'soltado'}")
                    if nombre == "ps" and presionado:
                        procesar_comando("MODE:GUI" if estado["modo"] == "MANUAL" else "MODE:MANUAL", "mando")
                    elif nombre == "options" and presionado:
                        procesar_comando("ESTOP", "mando")
                    elif estado["modo"] != "MANUAL":
                        if presionado:
                            print("   (Ignorado: modo GUI. Presiona PS o usa el switch de la App)")
                    elif nombre == "x":
                        procesar_comando("BUZ:1" if presionado else "BUZ:0", "mando")
                    elif presionado and nombre in acciones:
                        procesar_comando(acciones[nombre], "mando")
                elif ev.type == ecodes.EV_ABS:
                    if ev.code == ecodes.ABS_HAT0Y and ev.value != 0 and estado["modo"] == "MANUAL":
                        paso = PASO_VELOCIDAD if ev.value < 0 else -PASO_VELOCIDAD
                        print(f"\n<- Mando: cruceta {'arriba' if paso > 0 else 'abajo'}")
                        procesar_comando(f"SPD:{paso:+d}", "mando")
                    elif ev.code in rangos:
                        lo, hi = rangos[ev.code]
                        ejes[nombres_ejes[ev.code]] = ((ev.value - lo) / (hi - lo)) * 2 - 1 if hi > lo else 0.0
                        if estado["modo"] != "MANUAL":
                            continue
                        x = max(-1.0, min(1.0, ejes["lx"] + ejes["rx"]))
                        y = max(-1.0, min(1.0, ejes["ly"] + ejes["ry"]))
                        izq, der = mezcla(x, -y)
                        if ((izq, der) == (0, 0) and ultimo != (0, 0)
                                or abs(izq - ultimo[0]) + abs(der - ultimo[1]) >= 4):
                            ultimo = (izq, der)
                            procesar_comando(f"DRV:{izq},{der}", "mando")
        except OSError:
            pass
        finally:
            dev.close()
        print("\nMando PS4 desconectado")
        aviso = True
        with candado:
            if estado["modo"] == "MANUAL" and not estado["parada"]:
                _detener_conduccion()
                _poner("motor1", "M1:1", "M1:0", False)
                _poner("motor2", "M2:1", "M2:0", False)
                _poner("buzzer", "BUZ:1", "BUZ:0", False)
                print("   Seguridad: motores y buzzer apagados")
                imprimir_estado()

# CONFIGURACIÓN BLUETOOTH 
import os
if os.geteuid() != 0:
    print("\n*** AVISO: ejecuta con sudo -> sudo python3 control_auto.py ***")
    print("*** Sin sudo App Inventor puede no encontrar el canal Bluetooth ***\n")
try:
    if "Serial Port" not in subprocess.run(["sdptool", "browse", "local"],
                                           capture_output=True, text=True, timeout=5).stdout:
        r = subprocess.run(["sdptool", "add", f"--channel={CANAL_RFCOMM}", "SP"],
                           capture_output=True, timeout=5)
        if r.returncode != 0:
            print("Aviso: no se pudo registrar el perfil Serial Port. Ejecuta con sudo.")
except Exception as e:
    print(f"Aviso: no se pudo registrar el perfil Serial Port ({e}). Ejecuta con sudo.")

server_sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_sock.bind(("00:00:00:00:00:00", CANAL_RFCOMM))
server_sock.listen(1)

threading.Thread(target=leer_tiva, daemon=True).start()
threading.Thread(target=mantener_conduccion, daemon=True).start()
threading.Thread(target=hilo_mando, daemon=True).start()

enviar_a_tiva(traducir_comando(f"SPD:{VELOCIDAD_INICIAL}"))

print("\n=== SERVIDOR BLUETOOTH INICIADO ===")
imprimir_estado()

try:
    while True:
        print("Esperando conexión desde tu celular (App Inventor)...")
        client_sock, client_info = server_sock.accept()
        print(f"¡Teléfono conectado exitosamente desde!: {client_info}")
        pendiente = ""

        try:
            while True:
                data = client_sock.recv(1024).decode('utf-8', 'ignore')
                if not data:
                    break

                pendiente += data.replace('\r', '\n')
                if not pendiente.endswith('\n'):
                    client_sock.settimeout(0.1)
                    try:
                        mas = client_sock.recv(1024).decode('utf-8', 'ignore')
                        if not mas:
                            break
                        pendiente += mas.replace('\r', '\n')
                    except socket.timeout:
                        pendiente += '\n'
                    finally:
                        client_sock.settimeout(None)
                comandos = pendiente.split('\n')
                pendiente = comandos.pop()
                for cmd_app in comandos:
                    cmd_app = cmd_app.strip()
                    if cmd_app:
                        print(f"\n<- Recibido de la App: '{cmd_app}'")

                        if traducir_comando(cmd_app) is None:
                            print(f"   (Instrucción '{cmd_app}' gestionada en Raspberry)")
                        respuesta = procesar_comando(cmd_app, "app")
                        try:
                            client_sock.send((respuesta + "\n").encode('utf-8'))
                        except OSError:
                            pass
        except OSError as e:
            print(f"Conexión con el teléfono perdida: {e}")
        finally:
            client_sock.close()
            print("Teléfono desconectado.")

except KeyboardInterrupt:
    print("\nDeteniendo servidor...")

finally:
    corriendo = False
    try:
        enviar_a_tiva("DRV:0,0")
    except Exception:
        pass
    if 'client_sock' in locals():
        client_sock.close()
    server_sock.close()
    if 'puerto_tiva' in locals() and puerto_tiva.is_open:
        puerto_tiva.close()
    print("Conexiones Bluetooth y Serial cerradas correctamente.")