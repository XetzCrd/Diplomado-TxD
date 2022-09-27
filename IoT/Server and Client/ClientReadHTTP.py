import requests

#url = "https://drive.google.com/file/d/1I9n-wZKZZoCWKz8rnKkltGNh4Wsvq93g/view123"
url = "http://192.168.1.17:5000" #me conecto al puerto que abrí del servidor en mi pc

r = requests.get(url)

print(r.status_code)

if r.status_code >= 200 and r.status_code < 300:
    print(r.text)