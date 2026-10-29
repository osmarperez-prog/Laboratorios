import cv2

def resize_img(img, width, height):
    up_points = (width, height)
    img_resize = cv2.resize(img, up_points)
    return img_resize

if __name__ == "__main__":
    numero=input("elige un tamaño: ")
    

    if(numero=="1"):
        img = cv2.imread("im1.jpeg")

        rimg = resize_img(img, 300, 300)
        cv2.imshow("resize image", rimg)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    elif(numero=="2"):
        img = cv2.imread("im1.jpeg")

        rimg = resize_img(img, 600, 600)
        cv2.imshow("resize image", rimg)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    elif(numero=="3"):
        img = cv2.imread("im1.jpeg")

        rimg = resize_img(img, 1000, 1000)
        cv2.imshow("resize image", rimg)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    elif(numero=="4"):
        img = cv2.imread("im1.jpeg")

        cv2.imshow("Imagen original", img)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    else:
        print("opcion no valida")
