import cv2


# FUNCION PARA ROTAR 90 GRADOS HACIA LA DERECHA

def rotate_img(img):
    img_rotate = cv2.rotate(
        img,
        cv2.ROTATE_90_CLOCKWISE
    )

    return img_rotate


# PROGRAMA PRINCIPAL

if __name__ == "__main__":

    img = cv2.imread("2472432.jpg")

    if img is None:
        print("No se encontro la imagen 2472432.jpg")

    else:
        angulo = 0

        cv2.namedWindow(
            "Imagen rotada",
            cv2.WINDOW_NORMAL
        )

        print("Selecciona la ventana de la imagen.")
        print("Pulsa una tecla para girar 90 grados.")
        print("Pulsa Esc para salir.")

        while True:
            # Mostrar la orientacion actual.
            cv2.imshow("Imagen rotada", img)

            # Esperar una pulsacion en la ventana.
            tecla = cv2.waitKey(0)

            # Esc permite terminar el programa.
            # -1 permite salir si la espera termina sin una tecla.
            if tecla == 27 or tecla == -1:
                break

            # Rotar sobre el resultado anterior.
            img = rotate_img(img)

            angulo = (angulo + 90) % 360

            print(f"Orientacion actual: {angulo} grados")

        cv2.destroyAllWindows()