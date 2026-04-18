import os
import requests
from flask import Flask, send_from_directory, request, jsonify

app = Flask(__name__)

ESP32_URL = os.environ.get("ESP32_URL", "http://4wd-car.local")


@app.route("/")
def index():
    return send_from_directory("static", "index.html")


@app.route("/api/drive")
def drive():
    action = request.args.get("action", "stop")
    speed = request.args.get("speed", "255")
    try:
        r = requests.get(
            f"{ESP32_URL}/drive",
            params={"action": action, "speed": speed},
            timeout=0.5,
        )
        return r.text, r.status_code
    except requests.RequestException as e:
        return jsonify({"error": str(e)}), 502


@app.route("/api/stop")
def stop():
    try:
        r = requests.get(f"{ESP32_URL}/stop", timeout=0.5)
        return r.text, r.status_code
    except requests.RequestException as e:
        return jsonify({"error": str(e)}), 502


@app.route("/api/status")
def status():
    try:
        r = requests.get(f"{ESP32_URL}/status", timeout=0.5)
        return r.json(), r.status_code
    except requests.RequestException:
        return jsonify({"error": "ESP32 unreachable"}), 502


if __name__ == "__main__":
    print(f"ESP32 target: {ESP32_URL}")
    print("Starting server on http://0.0.0.0:8080")
    app.run(host="0.0.0.0", port=9999)
