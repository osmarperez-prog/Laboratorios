import cv2

if __name__ == "__main__":

    camara = cv2.VideoCapture(0)

    modo = "RGB"

    while True:

        check, frame = camara.read()

        if not check:
            print("Error al leer la camara")
            break

        if modo == "GRIS":
            imagen = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        elif modo == "RGB":
            imagen = frame

        cv2.imshow("Camara", imagen)

        tecla = cv2.waitKey(1) & 0xFF

        if tecla == ord("g"):
            modo = "GRIS"

        elif tecla == ord("r"):
            modo = "RGB"

        elif tecla == 27:
            break

    camara.release()

    cv2.destroyAllWindows()