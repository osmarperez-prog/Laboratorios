import cv2

def detectar_color(img):

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    rojo1 = cv2.inRange(hsv, (0, 100, 100), (10, 255, 255))
    rojo2 = cv2.inRange(hsv, (170, 100, 100), (179, 255, 255))
    rojo = rojo1 | rojo2

    verde = cv2.inRange(hsv, (30, 50, 40), (90, 255, 255))

    azul = cv2.inRange(hsv, (100, 100, 100), (130, 255, 255))

    return rojo, verde, azul


if __name__ == "__main__":

    img = cv2.imread("colors/rojo.jpg")

    if img is None:
        print("Error al cargar la imagen")

    else:
        rojo, verde, azul = detectar_color(img)

        print("Pixeles rojos:", cv2.countNonZero(rojo))
        print("Pixeles verdes:", cv2.countNonZero(verde))
        print("Pixeles azules:", cv2.countNonZero(azul))

        if cv2.countNonZero(rojo) > 0:
            print("Color rojo detectado")

        if cv2.countNonZero(verde) > 0:
            print("Color verde detectado")

        if cv2.countNonZero(azul) > 0:
            print("Color azul detectado")

        imagen_rojo = cv2.bitwise_and(img, img, mask=rojo)
        imagen_verde = cv2.bitwise_and(img, img, mask=verde)
        imagen_azul = cv2.bitwise_and(img, img, mask=azul)

        cv2.imshow("Imagen original", img)

        cv2.imshow("Color rojo", imagen_rojo)
        cv2.imshow("Color verde", imagen_verde)
        cv2.imshow("Color azul", imagen_azul)

        cv2.waitKey(0)
        cv2.destroyAllWindows()