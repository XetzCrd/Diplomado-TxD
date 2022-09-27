from sqlite3 import Timestamp
from fastapi import FastAPI
import uvicorn
from datetime import datetime
from pydantic import BaseModel

app = FastAPI()

@app.get("/")
async def index():
    return {"message": "Hello"}

#===========================================================================#
class IMU(BaseModel):
    ax: float
    ay: float
    az: float
    gx: float
    gy: float
    gz: float
    timestamp : str

@app.post("/IMU")
async def crea_dato_sensor_distancia(payload: IMU):
    print(f"{payload.ax}, {payload.ay}, {payload.az}, {payload.gx}, {payload.gy}, {payload.gz}, {payload.timestamp}")

    with open("imu.csv", "r") as f:
        contenido = f.read()
    
    contenido += f"{payload.ax}, {payload.ay}, {payload.az}, {payload.gx}, {payload.gy}, {payload.gz}, {payload.timestamp}\n"

    with open("imu.csv", "w+") as f:
        f.write(contenido)
    
    return {"message": "Dato recibido"}

#===========================================================================#
class Payload(BaseModel):
    data : str
    timestamp : str

@app.post("/sensor")
async def crea_dato_sensor(payload: Payload):
    print(f"Dato es {payload.data} y timestamp es {payload.timestamp}")

    return {"message": "Dato recibido"}

#===========================================================================#
class SensorDistancia(BaseModel):
    data: int

@app.post("/sensor/distancia")
async def crea_dato_sensor_distancia(payload: SensorDistancia):
    print(f"Dato es {payload.data}")

    with open("datos.csv", "r") as f:
        contenido = f.read()
    
    contenido += f"{payload.data}\n"

    with open("datos.csv", "w+") as f:
        f.write(contenido)
    
    return {"message": "Dato recibido"}

#===========================================================================#
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=5000) #inicializa el servidor