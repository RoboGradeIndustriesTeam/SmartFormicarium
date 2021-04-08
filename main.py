from flask import Flask, render_template
from dotenv import load_dotenv
import json
import os
import requests

load_dotenv()

app = Flask(__name__)


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
def index():
    return render_template('index.html')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80, debug=True)
