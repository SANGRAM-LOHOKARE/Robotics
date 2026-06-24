import cv2 as cv    
import numpy as np
cam = cv.VideoCapture(0)
while True:
    _, frame = cam.read()
    frame = cv.resize(cv.flip(frame, 1), (1200, 900))
    gray = cv.resize(cv.cvtColor(frame, cv.COLOR_BGR2GRAY), (800, 800))
    #haarcascade_frontalface_default.xml github link: https://github.com/opencv/opencv/blob/master/data/haarcascades/haarcascade_frontalface_default.xml
    faces = cv.CascadeClassifier('face_detect.xml').detectMultiScale(gray, 1.1, 4)
    for (x,y,w,h) in faces:
        cv.rectangle(frame, (x,y), (x+w, y+h), (0,255,0), 2)
    cv.imshow('frame', frame)
    if cv.waitKey(1) & 0xFF == ord('q'):
        break
cam.release()
cv.destroyAllWindows()