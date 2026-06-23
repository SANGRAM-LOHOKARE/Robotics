import cv2 as cv
import numpy as np

#colors
COLOR_RANGES = {
    "RED":      [(np.array([0, 130, 80]),    np.array([7, 255, 255])),  (np.array([173, 130, 80]),  np.array([180, 255, 255]))],
    "ORANGE":   [(np.array([8, 140, 100]),   np.array([21, 255, 255]))],
    "YELLOW":   [(np.array([22, 100, 120]),  np.array([35, 255, 255]))],
    "GREEN":    [(np.array([36, 75, 45]),    np.array([88, 255, 255]))],
    "BLUE":     [(np.array([100, 120, 50]),  np.array([126, 255, 255]))],
    "VIOLET":   [(np.array([127, 100, 70]),  np.array([142, 255, 255]))],
    "PURPLE":   [(np.array([143, 85, 45]),   np.array([160, 255, 255]))],
    "PINK":     [(np.array([161, 80, 90]),   np.array([172, 255, 255]))],
    "BROWN":    [(np.array([5, 80, 40]),     np.array([18, 170, 110]))],
    "WHITE":    [(np.array([0, 0, 205]),     np.array([180, 35, 255]))],
    "GRAY":     [(np.array([0, 0, 50]),      np.array([180, 40, 204]))],
    "BLACK":    [(np.array([0, 0, 0]),       np.array([180, 255, 49]))]
}
#Boxes
BOX_COLORS = {
    "RED":          (0, 0, 255),       # Bright Red
    "ORANGE":       (0, 128, 255),     # Orange
    "YELLOW":       (0, 255, 255),     # Yellow
    "GREEN":        (0, 255, 0),       # Green
    "BLUE":         (255, 0, 0),       # Royal Blue
    "VIOLET":       (255, 100, 180),   # Light bluish-purple
    "PURPLE":       (160, 32, 240),    # Traditional deep purple
    "PINK":         (180, 105, 255),   # Pink
    "BROWN":        (42, 42, 165),     # Brown
    "WHITE":        (240, 240, 240),   # Off-White
    "GRAY":         (128, 128, 128),   # Gray
    "BLACK":        (45, 45, 45)       # Charcoal Dark
}

#________________________________________________________________________________________________________
def detect_colors(frame):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    results = []

    for color_name, ranges in COLOR_RANGES.items():
        mask = np.zeros(hsv.shape[:2], dtype=np.uint8)  #black canvas
        for (lower, upper) in ranges:
            mask |= cv.inRange(hsv, lower, upper)
          #  cv.imshow(f"Mask - {color_name}", mask)  # Show individual color masks for debugging

        # Advanced Noise Filtering
        kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (5, 5))
        mask = cv.morphologyEx(mask, cv.MORPH_OPEN,  kernel, iterations=2)
        mask = cv.morphologyEx(mask, cv.MORPH_CLOSE, kernel, iterations=2)

        contours, _ = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

        for cnt in contours:
            area = cv.contourArea(cnt)
            if area > 1000:     # Box area threshold to avoid noise
                x, y, w, h = cv.boundingRect(cnt)
                results.append({
                    "color": color_name,
                    "x": x, "y": y,
                    "w": w, "h": h,
                    "area": int(area)
                })
    return results

#________________________________________________________________________________________________________
def draw_detections(frame, detections):
    for det in detections:
        color_name = det["color"]
        box_color  = BOX_COLORS.get(color_name, (255, 255, 255))
        x, y, bw, bh = det["x"], det["y"], det["w"], det["h"]

        cv.rectangle(frame, (x, y), (x + bw, y + bh), box_color, 2)

        #Text above box or below if too close to top of screen 
        label = color_name.replace("_", " ")
        label_y = y - 7 if y - 7 > 15 else y + bh + 20
        
        cv.putText(frame, label, (x, label_y), cv.FONT_HERSHEY_SIMPLEX, 0.6, box_color, 2, cv.LINE_AA)

    return frame

#________________________________________________________________________________________________________
def main():
    cam = cv.VideoCapture(0)
    
    cam.set(cv.CAP_PROP_FRAME_WIDTH, 1280)
    cam.set(cv.CAP_PROP_FRAME_HEIGHT, 720)

    if not cam.isOpened():          # Exception handling for camera offline
        print(f"Error: Camera offline.")
        return

    while True:
        ret, frame = cam.read()
        if not ret or frame is None:
            print(f"Error: Failed to read frame.")
            break
        frame = cv.flip(frame, 1)

        detections = detect_colors(frame)
        processed = draw_detections(frame, detections)

        cv.imshow("Color Detection (press 'q' to Quit)", processed)

        if cv.waitKey(1) & 0xFF == ord('q'):
            break
            
    cam.release()
    cv.destroyAllWindows()
    
if __name__ == '__main__':
    main()