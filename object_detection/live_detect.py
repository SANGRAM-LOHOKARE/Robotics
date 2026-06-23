import cv2
from ultralytics import YOLO

# 1. Load your brand-new custom trained AirPods model
# Using the exact directory where your terminal saved the weights
model = YOLO(r"D:\arduino\opencvv\runs\detect\train-2\weights\best.pt")

# 2. Fire up the webcam (0 is usually the integrated webcam)
cap = cv2.VideoCapture(0)

# Check if webcam opened correctly
if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

print("Webcam live! Press 'q' to quit ")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab camera frame.")
        break

    # 3. Predict on the current frame
    # stream=True optimizes generator-based memory usage for live frame rates
    results = model(frame, stream=True)

    # 4. Parse detections and draw bounding boxes using OpenCV
    for r in results:
        boxes = r.boxes
        for box in boxes:
            # Get rectangle coordinates
            x1, y1, x2, y2 = box.xyxy[0]
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

            confidence = float(box.conf[0]) # Extract confidence score

            # Get class name (your annotated label)
            class_id = int(box.cls[0])
            class_name = model.names[class_id]

            # Only show bounding boxes if the model is over 45% confident
            if confidence > 0.45:
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 3)    #green rectangle

                # Generate the text string
                label_text = f"{class_name} {confidence * 100:.1f}%"
                
                # Overlay text inside the backdrop banner
                cv2.putText(frame, label_text, (x1 + 5, y1 - 7), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)

    # 5. Display the window
    cv2.imshow("Object Tracking/detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()