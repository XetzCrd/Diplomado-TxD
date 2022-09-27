from datetime import datetime
import requests

#url = "http://localhost:5000/sensor"
url = "http://192.168.1.17:5000/sensor" #me conecto al puerto que abrí del servidor en mi pc

payload = {"data": "10", "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S")}

print(payload)

res = requests.post(url, json=payload)

print(res.status_code)

if res.status_code >= 200 and res.status_code < 300:
    print(res.text)

if res.status_code == 422:
    print(res.json())