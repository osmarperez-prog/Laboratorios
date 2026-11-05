import cv2

class ConvertirColor:

    def __init__(self, img):
        self.img = img

    def convertir_rgb(self):
        rgb = cv2.cvtColor(self.img, cv2.COLOR_BGR2RGB)
        return rgb

    def convertir_hsv(self):
        hsv = cv2.cvtColor(self.img, cv2.COLOR_BGR2HSV)
        return hsv

    def convertir_gris(self):
        gris = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)
        return gris


if __name__ == "__main__":

    img = cv2.imread("colors/rojo.jpg")

    color = ConvertirColor(img)

    rgb = color.convertir_rgb()
    hsv = color.convertir_hsv()
    gris = color.convertir_gris()

    print("RGB:", rgb[0, 0])
    print("HSV:", hsv[0, 0])
    print("GRIS:", gris[0, 0])

    cv2.imshow("Imagen original", img)
    cv2.imshow("Imagen RGB", cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR))
    cv2.imshow("Imagen HSV", cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR))
    cv2.imshow("Imagen gris", gris)

    cv2.waitKey(0)
    cv2.destroyAllWindows()