import cv2


# FUNCION PARA CAMBIAR EL TAMANO

def resize_img(img, width, height):
    return cv2.resize(img, (width, height))


# MOSTRAR LAS IMAGENES DE UNA ETAPA

def mostrar_imagenes(imagenes):
    for titulo, imagen in imagenes.items():
        cv2.namedWindow(
            titulo,
            cv2.WINDOW_NORMAL
        )

        cv2.imshow(titulo, imagen)

    print("Selecciona una ventana y pulsa una tecla para continuar.")

    cv2.waitKey(0)
    cv2.destroyAllWindows()


# PROGRAMA PRINCIPAL

if __name__ == "__main__":

    img = cv2.imread("2472432.jpg")

    if img is None:
        print("No se encontro la imagen 2472432.jpg")

    else:
        # 1. MOSTRAR LA IMAGEN ORIGINAL

        print("\nETAPA 1: Imagen original")

        mostrar_imagenes({
            "Original": img
        })


        # 2. CAMBIAR A 400 DE ANCHO Y 600 DE ALTO

        img = resize_img(img, 400, 600)

        alto, ancho = img.shape[:2]

        print("\nETAPA 2: Imagen redimensionada")
        print(f"Ancho: {ancho}, alto: {alto}")

        mostrar_imagenes({
            "Imagen de 400 x 600": img
        })


        # CALCULAR LOS PUNTOS CENTRALES

        mitad_alto = alto // 2
        mitad_ancho = ancho // 2


        # 3. DIVIDIR HORIZONTALMENTE
        # Se obtienen una parte superior y otra inferior.

        superior = img[:mitad_alto, :]
        inferior = img[mitad_alto:, :]

        print("\nETAPA 3: Division horizontal")

        mostrar_imagenes({
            "Mitad superior": superior,
            "Mitad inferior": inferior
        })


        # 4. DIVIDIR VERTICALMENTE
        # Se obtienen una parte izquierda y otra derecha.

        izquierda = img[:, :mitad_ancho]
        derecha = img[:, mitad_ancho:]

        print("\nETAPA 4: Division vertical")

        mostrar_imagenes({
            "Mitad izquierda": izquierda,
            "Mitad derecha": derecha
        })


        # 5. DIVIDIR EN CUATRO CUADRANTES

        cuadrante1 = img[
            :mitad_alto,
            :mitad_ancho
        ]

        cuadrante2 = img[
            :mitad_alto,
            mitad_ancho:
        ]

        cuadrante3 = img[
            mitad_alto:,
            :mitad_ancho
        ]

        cuadrante4 = img[
            mitad_alto:,
            mitad_ancho:
        ]

        print("\nETAPA 5: Cuatro cuadrantes")
        print("Cada cuadrante tiene 200 de ancho y 300 de alto.")

        mostrar_imagenes({
            "Cuadrante 1": cuadrante1,
            "Cuadrante 2": cuadrante2,
            "Cuadrante 3": cuadrante3,
            "Cuadrante 4": cuadrante4
        })

        print("\nEjercicio terminado.")