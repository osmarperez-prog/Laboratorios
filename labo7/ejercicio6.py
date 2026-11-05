import cv2

def convertir_gris(img):
    gris = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return gris

if __name__ == "__main__":

    rojo = cv2.imread("colors/rojo.jpg")
    verde = cv2.imread("colors/verde.jpg")
    azul = cv2.imread("colors/azul.jpg")

    gris_rojo = convertir_gris(rojo)
    gris_verde = convertir_gris(verde)
    gris_azul = convertir_gris(azul)

    print("Rojo en gris:", gris_rojo[0, 0])
    print("Verde en gris:", gris_verde[0, 0])
    print("Azul en gris:", gris_azul[0, 0])

    cv2.imshow("Rojo en gris", gris_rojo)
    cv2.imshow("Verde en gris", gris_verde)
    cv2.imshow("Azul en gris", gris_azul)

    cv2.waitKey(0)
    cv2.destroyAllWindows()