from flask import Flask, render_template, request
from dotenv import load_dotenv
import json
import os
import requests

load_dotenv()

app = Flask(__name__, template_folder='web', static_folder='assets')


def req_fan(fan_number=0, action=0):
    data = {
        "cmd": "fan",
        "data": {
            "fan_number": fan_number,
            "action": action
        }
    }
    return requests.post(os.getenv("FORMICARIUM_IP"), data=json.dumps(data)).json()


def req_light(action=0):
    data = {
        "cmd": "light",
        "data": {
            "action": action
        }
    }
    return requests.post(os.getenv("FORMICARIUM_IP"), data=json.dumps(data)).json()


def req_sensor(sensor_name="dht"):
    data = {
        "cmd": "sensor",
        "data": {
            "name": sensor_name
        }
    }
    return requests.post(os.getenv("FORMICARIUM_IP"), data=json.dumps(data)).json()


def req_set(type="hum", value=0):
    data = {
        "cmd": "set",
        "data": {
            "type": type,
            "value": value
        }
    }
    return requests.post(os.getenv("FORMICARIUM_IP"), data=json.dumps(data)).json()


@app.route('/')
@app.route('/index')
@app.route('/index.html')
@app.route('/dashboard')
@app.route('/dashboard.html')
def index():
    return render_template('dashboard.html')


@app.route('/climat', methods=["GET", "POST"])
@app.route('/climat.html', methods=["GET", "POST"])
def climat():
    print(dict(request.form))
    print(req_sensor("dht"))
    if request.method == "POST":
        if request.form.get("valueHum") is not None:
            req_set("hum", value=req_sensor("dht")["sensorHum"] + int(request.form.get("valueHum")))
        elif request.form.get("valueTemp") is not None:
            req_set("temp", value=req_sensor("dht")["sensorTemp"] + int(request.form.get("valueTemp")))
    return render_template('climat.html')

@app.route('/stream')
@app.route('/stream.html')
def stream():
    return render_template('stream.html')

app.jinja_env.globals.update(req_set=req_set, req_fan=req_fan, req_sensor=req_sensor, req_light=req_light, os=os)
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80, debug=True)
