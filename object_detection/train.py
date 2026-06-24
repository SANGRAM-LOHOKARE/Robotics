import os
from roboflow import Roboflow
from ultralytics import YOLO

if __name__ == '__main__':
    # 1. PASTE YOUR EXACT ROBOFLOW CODE HERE TO AUTO-DOWNLOAD DATA
    key = os.getenv("ROBOFLOW_API_KEY")
    rf = Roboflow(api_key=key)
    project = rf.workspace("sangrams-workspace-awhwb").project("my-first-project-1tse1")
    version = project.version(1)
    dataset = version.download("yolov8")
                    
    # 2. Get the path to the downloaded data.yaml file
    yaml_path = os.path.join(dataset.location, "data.yaml")

    # 3. Load a YOLOv8 Nano model
    model = YOLO("yolov8n.pt") 

    # 4. Start the training process
    model.train(
        data=yaml_path, 
        epochs=30,        # epochs=30 tells the model to study your 15 images 30 times over
        imgsz=640, 
        device="cpu",
        workers=0,
        batch=4           # Prevents memory overload
)
    print("Training finished! Your custom weights are ready at: runs/detect/train/weights/best.pt")