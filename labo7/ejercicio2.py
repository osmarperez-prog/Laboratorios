import cv2


# FUNCION PARA CAMBIAR EL TAMANO

def resize_img(img, width, height):
    up_points = (width, height)

    img_resize = cv2.resize(
        img,
        up_points
    )

    return img_resize


# PROGRAMA PRINCIPAL

if __name__ == "__main__":

    # La imagen debe estar en la carpeta desde la que ejecutas.
    img = cv2.imread("2472432.jpg")

    if img is None:
        print("No se encontro la imagen 2472432.jpg")

    else:
        print("SELECCIONA EL TAMANO")
        print("1. Original")
        print("2. Pequeno: 250 x 250")
        print("3. Mediano: 500 x 500")
        print("4. Grande: 1000 x 1000")

        while True:
            opcion = input("Elige una opcion: ").strip()

            if opcion == "1":
                rimg = img.copy()
                break

            elif opcion == "2":
                rimg = resize_img(img, 250, 250)
                break

            elif opcion == "3":
                rimg = resize_img(img, 500, 500)
                break

            elif opcion == "4":
                rimg = resize_img(img, 1000, 1000)
                break

            else:
                print("Opcion incorrecta. Elige del 1 al 4.")

        # shape devuelve primero alto y despues ancho.
        alto, ancho = rimg.shape[:2]

        print(f"Tamano seleccionado: {ancho} x {alto}")

        cv2.namedWindow(
            "resize image",
            cv2.WINDOW_NORMAL
        )

        cv2.imshow("resize image", rimg)

        cv2.waitKey(0)
        cv2.destroyAllWindows()
