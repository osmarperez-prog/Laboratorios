import os

# Configuracion grafica para Ubuntu en VirtualBox.
os.environ["QT_QPA_PLATFORMTHEME"] = "generic"
os.environ["QT_STYLE_OVERRIDE"] = "Fusion"
os.environ["LIBGL_ALWAYS_SOFTWARE"] = "true"
os.environ["GALLIUM_DRIVER"] = "llvmpipe"

import cv2


# CLASE QUE CONTROLA LA CAMARA Y SUS FILTROS

class CamaraConFiltros:

    def __init__(self, indice=0):
        # Indice de la camara que vamos a utilizar.
        self.indice = indice

        self.camara = None

        # Comenzar con el filtro de escala de grises.
        self.filtro = 1

        self.nombres = {
            0: "Original",
            1: "Escala de grises",
            2: "Desenfoque",
            3: "Bordes"
        }


    # APLICAR EL FILTRO AL FOTOGRAMA ORIGINAL

    def aplicar_filtro(self, frame):

        if self.filtro == 1:
            # Convertir la imagen a escala de grises.
            return cv2.cvtColor(
                frame,
                cv2.COLOR_BGR2GRAY
            )

        elif self.filtro == 2:
            # Desenfocar con un filtro gaussiano.
            return cv2.GaussianBlur(
                frame,
                (15, 15),
                0
            )

        elif self.filtro == 3:
            # Convertir a gris antes de detectar los bordes.
            gris = cv2.cvtColor(
                frame,
                cv2.COLOR_BGR2GRAY
            )

            # Suavizar para reducir el ruido.
            suave = cv2.GaussianBlur(
                gris,
                (5, 5),
                0
            )

            # Detectar los bordes.
            return cv2.Canny(
                suave,
                100,
                200
            )

        # Opcion 0: mostrar el fotograma sin filtro.
        return frame


    # SELECCIONAR UN FILTRO CON EL TECLADO

    def seleccionar_filtro(self, tecla):

        opciones = {
            ord("0"): 0,
            ord("1"): 1,
            ord("2"): 2,
            ord("3"): 3
        }

        if tecla in opciones:
            self.filtro = opciones[tecla]

            print(
                "Modo seleccionado:",
                self.nombres[self.filtro]
            )


    # ABRIR LA CAMARA Y MOSTRAR EL VIDEO

    def iniciar(self):

        self.camara = cv2.VideoCapture(
            self.indice,
            cv2.CAP_V4L2
        )

        try:
            # Comprobar que la camara se abrio.
            if not self.camara.isOpened():
                print("No se pudo abrir la camara.")

                print(
                    "Revisa Dispositivos > Camaras web "
                    "en VirtualBox."
                )

                print(
                    "Comprueba en Ubuntu: ls /dev/video*"
                )

                return

            cv2.namedWindow(
                "Camara con filtros",
                cv2.WINDOW_NORMAL
            )

            print("Selecciona la ventana de la camara.")
            print("1: Escala de grises")
            print("2: Desenfoque")
            print("3: Bordes")
            print("0: Original sin filtro")
            print("Esc: Salir")

            print(
                "Modo inicial:",
                self.nombres[self.filtro]
            )

            while True:
                # Leer un nuevo fotograma.
                check, frame = self.camara.read()

                if not check or frame is None:
                    print("No se pudo leer el fotograma.")
                    break

                # Aplicar el filtro seleccionado.
                resultado = self.aplicar_filtro(frame)

                # Mostrar el resultado.
                cv2.imshow(
                    "Camara con filtros",
                    resultado
                )

                # Revisar el teclado sin detener el video.
                tecla = cv2.waitKey(1) & 0xFF

                if tecla == 27:
                    break

                self.seleccionar_filtro(tecla)

        finally:
            # Liberar los recursos al terminar.
            self.camara.release()
            cv2.destroyAllWindows()


# PROGRAMA PRINCIPAL

if __name__ == "__main__":
    programa = CamaraConFiltros(indice=0)

    programa.iniciar()