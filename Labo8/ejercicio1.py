import cv2

def resize_img(img, width, height):
    up_points = (width, height)
    img_resize = cv2.resize(img, up_points)
    return img_resize

def detectar_bordes(img):
    gris = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    bordes = cv2.Canny(gris, 100, 200)
    return bordes

def dividir_mitades(img):
    h, w = img.shape[:2]

    mitad_h = h // 2

    superior = img[0:mitad_h, :]
    inferior = img[mitad_h:h, :]

    return superior, inferior

def dividir_cuadrantes(img):
    h, w = img.shape[:2]

    mitad_h = h // 2
    mitad_w = w // 2

    cuadrante1 = img[0:mitad_h, 0:mitad_w]
    cuadrante2 = img[0:mitad_h, mitad_w:w]
    cuadrante3 = img[mitad_h:h, 0:mitad_w]
    cuadrante4 = img[mitad_h:h, mitad_w:w]

    return cuadrante1, cuadrante2, cuadrante3, cuadrante4

if __name__ == "__main__":

    cap = cv2.VideoCapture("video.avi")

    if not cap.isOpened():
        print("Error al abrir el video")

    else:

        while True:

            check, frame = cap.read()

            if not check:
                break

            img_resize = resize_img(frame, 600, 400)

            bordes = detectar_bordes(img_resize)

            superior, inferior = dividir_mitades(img_resize)

            cuadrante1, cuadrante2, cuadrante3, cuadrante4 = dividir_cuadrantes(img_resize)

            cv2.imshow("Video original", frame)

            cv2.imshow("Video 400x600", img_resize)

            cv2.imshow("Bordes", bordes)

            cv2.imshow("Mitad superior", superior)
            cv2.imshow("Mitad inferior", inferior)

            cv2.imshow("Cuadrante 1", cuadrante1)
            cv2.imshow("Cuadrante 2", cuadrante2)
            cv2.imshow("Cuadrante 3", cuadrante3)
            cv2.imshow("Cuadrante 4", cuadrante4)

            cv2.waitKey(17)

    cap.release()

    cv2.destroyAllWindows()