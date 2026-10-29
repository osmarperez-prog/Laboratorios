import cv2

if __name__ == "__main__":

    img = cv2.imread("im1.jpeg")

    cv2.imshow("Imagen original", img)

    img_resize = cv2.resize(img, (400, 600))

    cv2.imshow("Imagen redimensionada", img_resize)

    h, w = img_resize.shape[:2]

    mitad_h = h // 2

    superior = img_resize[0:mitad_h, :]
    inferior = img_resize[mitad_h:h, :]

    cv2.imshow("Mitad superior", superior)
    cv2.imshow("Mitad inferior", inferior)

    mitad_w = w // 2

    izquierda = img_resize[:, 0:mitad_w]
    derecha = img_resize[:, mitad_w:w]

    cv2.imshow("Mitad izquierda", izquierda)
    cv2.imshow("Mitad derecha", derecha)

    cuadrante1 = img_resize[0:mitad_h, 0:mitad_w]
    cuadrante2 = img_resize[0:mitad_h, mitad_w:w]
    cuadrante3 = img_resize[mitad_h:h, 0:mitad_w]
    cuadrante4 = img_resize[mitad_h:h, mitad_w:w]

    cv2.imshow("Cuadrante 1", cuadrante1)
    cv2.imshow("Cuadrante 2", cuadrante2)
    cv2.imshow("Cuadrante 3", cuadrante3)
    cv2.imshow("Cuadrante 4", cuadrante4)

    cv2.waitKey(0)
    cv2.destroyAllWindows()