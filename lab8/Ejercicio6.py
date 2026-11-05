import cv2
import time

cam = cv2.VideoCapture(0)
cam.set(3, 640)  
cam.set(4, 480) 

if not cam.isOpened():
    print("Error")
    exit()

t_antes = 0

while True:
    ok, img = cam.read()
    if not ok:
        print("Error de video")
        break

    gris = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    suave = cv2.GaussianBlur(gris, (5, 5), 0)
    bordes = cv2.Canny(suave, 50, 150)
    contornos, _ = cv2.findContours(bordes, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    cv2.drawContours(img, contornos, -1, (0, 255, 0), 2)

    t_ahora = time.time()
    fps = int(1 / (t_ahora - t_antes)) if (t_ahora - t_antes) > 0 else 0
    t_antes = t_ahora

    cv2.putText(img, f"FPS: {fps}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

    cv2.imshow("Contornos", img)
    cv2.imshow("Bordes", bordes)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cam.release()
cv2.destroyAllWindows()