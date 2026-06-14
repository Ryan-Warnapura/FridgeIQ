clodpoll
clodpoll.
Online

saltybacon [PGR],  — 5:28 AM
rough summary of our "fridgeiq" jamhacks 2026 project
clodpoll — 6:08 AM
the idea is a smart recipe system. and an extra page to our website, that calls our claude api and uses our database with food items + expiration dates to ask, what recipes can we make? add a chatbox to directly ask for specific stuff, for example, its breakfast, what can i make? the ai knows whats in the fridge using our database and gives us some recipes. it also specifically can make recipees for items that are closer to expiring, and give those higher priority.
saltybacon [PGR],  — 6:39 AM
@katzinsky
Image
https://github.com/s4ltybacon/jamhacks-test
Leo — 6:54 AM
Image
3:2 aspect ratio
Image
saltybacon [PGR],  — 6:58 AM
Image
katzinsky — 7:06 AM
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>FridgeIQ Final Sync</title>... (105 KB left)

FridgeIQ_FINAL_TO_BUY_SHOPPING.html
155 KB
saltybacon [PGR],  — 7:11 AM
let me sleep
Image
saltybacon [PGR],  — 7:32 AM
Image
katzinsky — 7:42 AM
ssh jamhacks@fridgeiq.local
source ~/fridgevenv/bin/activate
python pi_server.py
clodpoll
 pinned a message to this channel. See all pinned messages. — 7:42 AM
clodpoll
 pinned a message to this channel. See all pinned messages. — 7:42 AM
saltybacon [PGR],  — 7:48 AM
laptop html file, raspberry pi file, arduino file, esp32 screen (sss d1) file 
saltybacon [PGR],  — 8:03 AM
say info such as the timespeed increase in the fridge
Leo
 pinned a message to this channel. See all pinned messages. — 8:07 AM
katzinsky — 8:10 AM
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
katzinsky — 8:10 AM
pi_server.py
sketch_jun13e.ino
#include <Arduino_GFX_Library.h>
#include <PCA95x5.h>
#define TOUCH_MODULES_FT5x06
#include <TouchLib.h>
#include <WiFi.h>
#include <HTTPClient.h>

message.txt
30 KB
this is for the seeed studio sensecap indicator
saltybacon [PGR],  — 8:11 AM
first 30s: talk about the issue, why we want to solve. 
1 min, live demo, takes us to 1/2 point
katzinsky — 8:12 AM
sketch_june13a.ino
#include <WiFiS3.h>
#include <math.h>

char ssid[] = "Shaki Booki";
char pass[] = "Ifraan@082513";

message.txt
6 KB
this is for the ardunuo uno r4 and the light and temp sensorss
saltybacon [PGR],  — 8:18 AM
Image
﻿
#include <WiFiS3.h>
#include <math.h>

char ssid[] = "Shaki Booki";
char pass[] = "Ifraan@082513";

const int TEMP_PIN = A0;
const int LIGHT_PIN = A1;

const float B = 4275.0;
const float R0 = 100000.0;
const float ADC_MAX = 1023.0;

WiFiServer server(80);

int lightValue = 0;
int tempRaw = 0;
float tempC = 0;
float tempF = 0; 
String lightStatus = "STARTING";
String tempStatus = "STARTING";

unsigned long lastLightRead = 0;
unsigned long lastTempRead = 0;

const unsigned long LIGHT_INTERVAL = 1000;
const unsigned long TEMP_INTERVAL = 5000;

int readLightAverage() {
  long total = 0;
  for (int i = 0; i < 20; i++) { total += analogRead(LIGHT_PIN); delay(2); }
  return total / 20;
}

String getLightStatus(int value) {
  if (value < 50) return "VERY DARK";
  else if (value < 150) return "DARK";
  else if (value < 500) return "BRIGHT";
  else return "VERY BRIGHT";
}

int readTempMedian() {
  const int N = 15;
  int values[N];
  for (int i = 0; i < N; i++) { values[i] = analogRead(TEMP_PIN); delay(10); }
  for (int i = 0; i < N - 1; i++)
    for (int j = i + 1; j < N; j++)
      if (values[j] < values[i]) { int t = values[i]; values[i] = values[j]; values[j] = t; }
  return values[N / 2];
}

float rawToCelsius(int raw) {
  float R = R0 * (ADC_MAX / raw - 1.0);
  return 1.0 / ((log(R / R0) / B) + (1.0 / 298.15)) - 273.15;
}

String getTempStatus(float celsius) {
  if (celsius < 0 || celsius > 60) return "UNREALISTIC - CHECK SENSOR";
  else if (celsius < 10) return "COLD / FRIDGE TEMP";
  else if (celsius < 28) return "ROOM TEMP";
  else return "WARM";
}

void updateLight() { lightValue = readLightAverage(); lightStatus = getLightStatus(lightValue); }

void updateTemp() {
  tempRaw = readTempMedian();
  if (tempRaw <= 0 || tempRaw >= 1023) { tempStatus = "ERROR - CHECK TEMP SENSOR"; return; }
  tempC = rawToCelsius(tempRaw);
  tempF = tempC * 9.0 / 5.0 + 32.0;
  tempStatus = getTempStatus(tempC);
}

void sendData(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.print("{");
  client.print("\"light\":"); client.print(lightValue); client.print(",");
  client.print("\"lightStatus\":\""); client.print(lightStatus); client.print("\",");
  client.print("\"tempRaw\":"); client.print(tempRaw); client.print(",");
  client.print("\"tempC\":"); client.print(tempC, 2); client.print(",");
  client.print("\"tempF\":"); client.print(tempF, 2); client.print(",");
  client.print("\"tempStatus\":\""); client.print(tempStatus); client.print("\"");
  client.println("}");
}

void sendWebsite(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head><title>FridgeIQ Arduino</title>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<style>body{font-family:Arial;background:#eef8ff;text-align:center;padding:30px;color:#123;}");
  client.println(".card{background:white;border-radius:18px;padding:20px;margin:15px auto;max-width:420px;box-shadow:0 0 15px #bcd;}");
  client.println(".big{font-size:38px;font-weight:bold;color:#0876c9;}</style></head><body>");
  client.println("<h1>FridgeIQ Arduino</h1>");
  client.println("<div class='card'><h2>Light</h2><div class='big' id='l'>--</div><p id='ls'>Loading...</p></div>");
  client.println("<div class='card'><h2>Temperature</h2><div class='big'><span id='tc'>--</span> C</div><p id='ts'>Loading...</p></div>");
  client.println("<script>async function u(){try{let r=await fetch('/data?x='+Date.now());let d=await r.json();");
  client.println("document.getElementById('l').innerText=d.light;document.getElementById('ls').innerText=d.lightStatus;");
  client.println("document.getElementById('tc').innerText=d.tempC.toFixed(2);document.getElementById('ts').innerText=d.tempStatus;");
  client.println("}catch(e){}}u();setInterval(u,1000);</script></body></html>");
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  analogReadResolution(10);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Arduino IP: http://");
  Serial.println(WiFi.localIP());
  Serial.print("JSON: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/data");

  server.begin();
  updateLight();
  updateTemp();
  lastLightRead = lastTempRead = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastLightRead >= LIGHT_INTERVAL) {
    lastLightRead = now;
    updateLight();
    Serial.print("[LIGHT] "); Serial.print(lightValue); Serial.print(" | "); Serial.println(lightStatus);
  }

  if (now - lastTempRead >= TEMP_INTERVAL) {
    lastTempRead = now;
    updateTemp();
    Serial.print("[TEMP] "); Serial.print(tempC, 2); Serial.print("C | "); Serial.println(tempStatus);
  }

  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();
    if (request.indexOf("GET /data") >= 0) sendData(client);
    else sendWebsite(client);
    delay(1);
    client.stop();
  }
}