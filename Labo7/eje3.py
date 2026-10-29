import cv2

if __name__ == "__main__":

    img = cv2.imread("im1.jpeg")

    cv2.imshow("Imagen", img)

    while True:

        cv2.waitKey(0)

        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)

        cv2.imshow("Imagen", img)