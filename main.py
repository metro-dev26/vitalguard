from fastapi import FastAPI
from pydantic import BaseModel
import numpy as np
from scipy.signal import butter, filtfilt
from collections import deque
import uvicorn

app = FastAPI()

# Buffer to store incoming IR values
ppg_buffer = deque(maxlen=250)

class SensorData(BaseModel):
    ir: int
    red: int
    hr: float
    spo2: float

@app.get("/")
def root():
    return {"status": "VitalGuard backend running"}

@app.post("/ingest")
def ingest(data: SensorData):
    ppg_buffer.append(data.ir)
    print(f"IR: {data.ir} | HR: {data.hr} | SpO2: {data.spo2}")
    return {"status": "ok", "buffer_size": len(ppg_buffer)}

@app.get("/ppg")
def get_ppg():
    return {"ppg": list(ppg_buffer)}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)