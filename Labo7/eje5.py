import cv2
from abc import ABC, abstractmethod

class video_Capture_abs(ABC):
    @abstractmethod
    def display_camera(self):
        pass
    @abstractmethod
    def stop_display(self):
        pass
    @abstractmethod
    def camera_visualization(self):
        pass
class video_Capture(video_Capture_abs):
    def __init__(self, camera) -> None:
        self.camera = camera
        self.displayed = False
    def display_camera(self):
        self.displayed = True
        self.camera_visualization()
    def stop_display(self):
        self.displayed = False
    def camera_visualization(self):
        while self.displayed:
            check, frame = self.camera.read()
            cv2.imshow("camera", frame)
            key = cv2.waitKey(1)
            if key == 27: 
                self.stop_display()
if __name__ == "__main__":
    camera = cv2.VideoCapture(0)
    camera_object = video_Capture(camera)
    camera_object.display_camera()