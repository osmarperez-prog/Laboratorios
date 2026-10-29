import cv2
import os

if __name__ == "__main__":

    os.makedirs("Captures", exist_ok=True)

    camara = cv2.VideoCapture(0)

    modo = "RGB"
    contador = 1
    ultima_imagen = None

    if not camara.isOpened():
        print("Error al abrir la camara")

    else:

        while True:

            check, frame = camara.read()

            if not check:
                print("Error al leer la camara")
                break

            if modo == "GRIS":
                imagen = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

            elif modo == "RGB":
                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                imagen = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)

            cv2.imshow("Camara", imagen)

            tecla = cv2.waitKey(1) & 0xFF

            if tecla == ord("g"):
                modo = "GRIS"
                print("Modo escala de grises")

            elif tecla == ord("r"):
                modo = "RGB"
                print("Modo RGB")

            elif tecla == ord("c"):

                nombre = "Captures/image" + str(contador) + ".jpg"

                if cv2.imwrite(nombre, frame):

                    print("Imagen guardada:", nombre)

                    ultima_imagen = frame.copy()

                    contador = contador + 1

                else:
                    print("Error al guardar la imagen")

            elif tecla == 27:
                break

        camara.release()
        cv2.destroyAllWindows()

        if ultima_imagen is not None:

            gris = cv2.cvtColor(ultima_imagen, cv2.COLOR_BGR2GRAY)

            h, w = gris.shape[:2]

            mitad_h = h // 2
            mitad_w = w // 2

            cuadrante1 = gris[0:mitad_h, 0:mitad_w]
            cuadrante2 = gris[0:mitad_h, mitad_w:w]
            cuadrante3 = gris[mitad_h:h, 0:mitad_w]
            cuadrante4 = gris[mitad_h:h, mitad_w:w]

            cv2.imshow("Imagen capturada", ultima_imagen)

            cv2.imshow("Escala de grises", gris)

            cv2.imshow("Cuadrante 1", cuadrante1)
            cv2.imshow("Cuadrante 2", cuadrante2)
            cv2.imshow("Cuadrante 3", cuadrante3)
            cv2.imshow("Cuadrante 4", cuadrante4)

            cv2.waitKey(0)
            cv2.destroyAllWindows()

        else:
            print("No se capturo ninguna imagen")