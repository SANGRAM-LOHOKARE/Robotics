import cv2
from ultralytics import YOLO

# Load your custom trained  model
# Using the exact directory where your terminal saved the weights
model = YOLO(r"D:\github\robotics\Robotics\object_detection\runs\detect\train-2\weights\best.pt")

cap = cv2.VideoCapture(0)

if not cap.isOpened():          # exception handling if the webcam is not opened
    print("Error: Could not open webcam.")
    exit()
print("Webcam live! Press 'q' to quit ")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab camera frame.")
        break

    # predicts the objects in the frame using the YOLO model
    results = model(frame, stream=True)

    # draw bounding boxes 
    for r in results:
        boxes = r.boxes
        for box in boxes:
            x1, y1, x2, y2 = box.xyxy[0]    #rectangle coordinates
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

            confidence = float(box.conf[0]) # confidence score

            # Get class name (your annotated label)
            class_id = int(box.cls[0])
            class_name = model.names[class_id]

            if confidence > 0.45:
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 3)    #green rectangle
                # Generate the text string
                label_text = f"{class_name} {confidence * 100:.1f}%"
                cv2.putText(frame, label_text, (x1 + 5, y1 - 7), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)

    cv2.imshow("Object Tracking/detection", frame)      # Display the window

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()