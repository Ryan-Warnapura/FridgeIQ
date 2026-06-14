from flask import Flask, send_file, request, jsonify
from flask_cors import CORS
from picamera2 import Picamera2
import io
import os
import json

app = Flask(name)
CORS(app)

SYNC_FILE = os.path.join(os.path.dirname(file), 'inventory_sync.json')

@app.route('/capture')
def capture():
    cam = Picamera2()
    config = cam.create_still_configuration(main={"size": (1920, 1080)})
    cam.configure(config)
    cam.start()
    buffer = io.BytesIO()
    cam.capture_file(buffer, format="jpeg")
    cam.stop()
    cam.close()
    buffer.seek(0)
    return send_file(buffer, mimetype='image/jpeg')

@app.route('/inventory/sync', methods=['GET'])
def inventory_sync_get():
    if os.path.exists(SYNC_FILE):
        with open(SYNC_FILE) as f:
            data = f.read()
        return data, 200, {
            'Content-Type': 'application/json',
            'Access-Control-Allow-Origin': ''
        }
    return '[]', 200, {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': ''
    }

@app.route('/inventory/sync', methods=['POST'])
def inventory_sync_post():
    with open(SYNC_FILE, 'w') as f:
        f.write(request.data.decode('utf-8'))
    return 'ok', 200, {'Access-Control-Allow-Origin': ''}

@app.route('/inventory/sync', methods=['OPTIONS'])
def inventory_sync_options():
    r = app.make_default_options_response()
    r.headers['Access-Control-Allow-Origin'] = ''
    r.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
    r.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    return r

if name == 'main':
    app.run(host='0.0.0.0', port=5000)