import cv2

if __name__ == "__main__":

    rojo = cv2.imread("colors/rojo.jpg")
    verde = cv2.imread("colors/verde.jpg")
    azul = cv2.imread("colors/azul.jpg")

    color_rojo = rojo[0, 0]
    color_verde = verde[0, 0]
    color_azul = azul[0, 0]

    print("Color rojo:", color_rojo)
    print("Color verde:", color_verde)
    print("Color azul:", color_azul)

    cv2.imshow("Rojo", rojo)
    cv2.imshow("Verde", verde)
    cv2.imshow("Azul", azul)

    cv2.waitKey(0)
    cv2.destroyAllWindows()