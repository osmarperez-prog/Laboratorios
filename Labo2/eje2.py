from gpiozero import LED, Button
from time import sleep

class ContadorBinario:
    def __init__(self, pin_led0, pin_led1, pin_led2, pin_led3, pin_btn_inc, pin_btn_dec):
        # 4 LEDs individuales (Bit 0 = 1, Bit 1 = 2, Bit 2 = 4, Bit 3 = 8)
        self.led0 = LED(pin_led0)
        self.led1 = LED(pin_led1)
        self.led2 = LED(pin_led2)
        self.led3 = LED(pin_led3)
        
        # 2 Botones
        self.btn_inc = Button(pin_btn_inc, bounce_time=0.2)
        self.btn_dec = Button(pin_btn_dec, bounce_time=0.2)
        
        self.valor = 0
        self.actualizar_salida()

    def incrementar(self):
        self.valor += 1
        if self.valor > 15:
            self.valor = 0
        self.actualizar_salida()

    def decrementar(self):
        if self.valor > 0:
            self.valor -= 1
        self.actualizar_salida()

    def actualizar_salida(self):
        # Mostrar en consola
        binario = f"{self.valor:04b}"
        hexadecimal = f"{self.valor:X}"
        print(f"Decimal: {self.valor:2d} | Binario: {binario} | Hex: {hexadecimal}")

        # Controlar cada LED según su bit correspondiente usando operaciones bit a bit
        # Bit 0 (Valor 1)
        if (self.valor & 1) != 0:
            self.led0.on()
        else:
            self.led0.off()

        # Bit 1 (Valor 2)
        if (self.valor & 2) != 0:
            self.led1.on()
        else:
            self.led1.off()

        # Bit 2 (Valor 4)
        if (self.valor & 4) != 0:
            self.led2.on()
        else:
            self.led2.off()

        # Bit 3 (Valor 8)
        if (self.valor & 8) != 0:
            self.led3.on()
        else:
            self.led3.off()

    def apagar_todo(self):
        self.led0.off()
        self.led1.off()
        self.led2.off()
        self.led3.off()


# --- PROGRAMA PRINCIPAL ---
if __name__ == "__main__":
    # Define aquí tus pines GPIO individuales
    contador = ContadorBinario(
        pin_led0=17, 
        pin_led1=27, 
        pin_led2=22, 
        pin_led3=5, 
        pin_btn_inc=6, 
        pin_btn_dec=13
    )

    print("Ejercicio 2 (POO sin listas) iniciado.")

    try:
        while True:
            if contador.btn_inc.is_pressed:
                contador.incrementar()
                while contador.btn_inc.is_pressed:
                    sleep(0.05)

            if contador.btn_dec.is_pressed:
                contador.decrementar()
                while contador.btn_dec.is_pressed:
                    sleep(0.05)

            sleep(0.05)

    except KeyboardInterrupt:
        contador.apagar_todo()
        print("\nPrograma finalizado de forma segura.")
