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

﻿
#include <Arduino_GFX_Library.h>
#include <PCA95x5.h>
#define TOUCH_MODULES_FT5x06
#include <TouchLib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

// =====================================================
// FridgeIQ SenseCAP Display - TOUCH VERSION
// SAFE NO-CANVAS BUILD: use this after recovery.
// Canvas build caused reboot/black screen on this setup.
// Fixes:
// 1) No full-screen redraw every inventory fetch
// 2) No HTTP fetching while the food list is scrolling
// 3) No slow wipe transitions
// 4) Sensor screen updates only the middle area
// 5) Auto Wi-Fi reconnect without freezing the display forever
// 6) Robust Arduino /data fetching with last-good-value fallback
// 7) Official SenseCAP touch method: TouchLib FT5x06 at 0x48 on SDA 39 / SCL 40
// 8) Touch-comfort tuning: slower drag, fractional scroll, and 60s auto-switch pause after touch
// =====================================================

// =======================
// WIFI + URLS
// =======================
const char* ssid     = "Shaki Booki";
const char* password = "Ifraan@082513";

const char* dataUrl  = "http://10.105.188.54/data";
const char* invUrl   = "http://10.105.188.113:5000/inventory/sync";

// =======================
// DISPLAY SETUP
// =======================
#define GFX_BL 45

// =======================
// OFFICIAL SENSECAP TOUCH / EXTENDER SETUP
// Based on Seeed/LongDirtyAnimAlf SenseCAP Arduino touch code:
// - Touch chip: FT5x06
// - Touch I2C address: 0x48
// - Extender I2C: SDA 39, SCL 40
// - Touch reset goes through PCA9555/PCA9535 port P07
// =======================
#define EXTENDER_SDA 39
#define EXTENDER_SCL 40
#define EXTENDER_SPEED 400000
#define TOUCH_MODULE_ADDR 0x48

#define EXPANDER_IO_RADIO_NSS   PCA95x5::Port::P00
#define EXPANDER_IO_RADIO_RST   PCA95x5::Port::P01
#define EXPANDER_IO_RADIO_BUSY  PCA95x5::Port::P02
#define EXPANDER_IO_RADIO_DIO_1 PCA95x5::Port::P03
#define EXPANDER_IO_LCD_CS      PCA95x5::Port::P04
#define EXPANDER_IO_LCD_RESET   PCA95x5::Port::P05
#define EXPANDER_IO_LCD_INT     PCA95x5::Port::P06
#define EXPANDER_IO_TP_RESET    PCA95x5::Port::P07
// NOTE: This PCA95x5 library build exposes only P00-P07, so extra P08/P10 lines are intentionally removed.

PCA9555 touchIoex;
bool extenderInitDone = false;
TouchLib touch(Wire, EXTENDER_SDA, EXTENDER_SCL, TOUCH_MODULE_ADDR);

// Change only these if touch direction is mirrored on your physical screen.
// Your touch test showed bottom-right touches drawing at top-left.
// That means both axes are mirrored, so X and Y must be inverted.
const bool TOUCH_SWAP_XY = false;
const bool TOUCH_INVERT_X = true;
const bool TOUCH_INVERT_Y = true;

bool touchReady = false;
bool touchDown = false;
int touchStartX = 0;
int touchStartY = 0;
int touchLastX = 0;
int touchLastY = 0;
int touchPrevDragY = 0;
unsigned long touchStartMs = 0;
unsigned long lastUserTouchMs = 0;
unsigned long lastTouchDrawMs = 0;

const unsigned long TOUCH_AUTOSCROLL_PAUSE = 30000;  // After any touch, pause all auto screen switching for 30 seconds
const int TAP_MAX_MOVE = 18;
const int SWIPE_MIN_DIST = 80;
const int TOUCH_DRAW_INTERVAL = 55;  // Safer redraw rate: less flicker, still responsive
const float TOUCH_SCROLL_SCALE = 0.22f;  // Slower touch scroll so it does not fly

Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED, PCA95x5::Port::P04, 41, 48, GFX_NOT_DEFINED
);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  18, 17, 16, 21,
  4, 3, 2, 1, 0,
  10, 9, 8, 7, 6, 5,
  15, 14, 13, 12, 11,
  1, 10, 8, 50,
  1, 10, 8, 20
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  480, 480,
  rgbpanel,
  2,
  true,
  bus,
  GFX_NOT_DEFINED,
  st7701_type1_init_operations,
  sizeof(st7701_type1_init_operations)
);

// =======================
// COLORS
// =======================
#define BG        0x0841
#define CARD      0x1082
#define CARD2     0x18C3
#define WHITE     0xFFFF
#define GRAY      0x8410
#define DARKGRAY  0x4208
#define GREEN     0x07E0
#define GREEN_DIM 0x03E0
#define RED       0xF800
#define AMBER     0xFD20
#define BLUE      0x039F
#define CYAN      0x07FF

// =======================
// SENSOR DATA
// =======================
int lightValue = 0;
float tempC = 0;
float tempF = 0;

String lightStatus = "---";
String tempStatus  = "---";

bool doorOpen = false;
bool wifiOk = false;
bool dataOk = false;
bool invOk = false;

// =======================
// INVENTORY DATA
// =======================
const int MAX_ITEMS = 32;
const int NAME_LEN = 32;

char itemNames[MAX_ITEMS][NAME_LEN];
float itemDaysLeft[MAX_ITEMS];
int itemCount = 0;

// =======================
// SCREEN + ANIMATION
// =======================
const int SCREEN_FOOD = 0;
const int SCREEN_SENSOR = 1;
int currentScreen = SCREEN_FOOD;

const int FOOD_LIST_TOP = 92;
const int FOOD_LIST_BOTTOM = 430;
const int FOOD_VIEW_HEIGHT = FOOD_LIST_BOTTOM - FOOD_LIST_TOP;
const int FOOD_ROW_HEIGHT = 72;

int scrollOffset = 0;
float scrollFloat = 0.0f;  // smooth/manual scroll position with fractional movement
int maxScroll = 0;

unsigned long lastScrollStep = 0;
unsigned long screenStartTime = 0;
unsigned long lastSensorFetch = 0;
unsigned long lastInventoryFetch = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastReconnectTry = 0;

const unsigned long SCROLL_INTERVAL = 78;        // Slower auto-scroll so it does not feel twitchy
const unsigned long SENSOR_SCREEN_TIME = 5200;   // Sensor page stays about 5 seconds
const unsigned long NO_SCROLL_FOOD_TIME = 6500;  // If only a few items, wait then switch
const unsigned long SENSOR_INTERVAL = 1000;      // Keep Arduino data fresh on sensor screen
const unsigned long INVENTORY_INTERVAL = 7000;   // Fetch Pi less often to avoid stutter
const unsigned long ARDUINO_HTTP_TIMEOUT = 1800; // Uno R4 server can be slow; do not timeout too fast
const unsigned long SENSOR_STALE_AFTER = 15000;  // Hold last good sensor data before showing offline
const unsigned long HEARTBEAT_INTERVAL = 700;
const unsigned long RECONNECT_INTERVAL = 9000;

bool heartbeatPulse = false;

String lastInvSignature = "";
String lastSensorSignature = "";

unsigned long lastGoodSensorMs = 0;
int sensorFailCount = 0;
int lastSensorHttpCode = 0;
String sensorDebug = "starting";
bool hasSensorEverWorked = false;


// =======================
// EXTENDER + TOUCH HELPERS
// =======================
void extender_init_once() {
  if (extenderInitDone) return;

  Wire.begin(EXTENDER_SDA, EXTENDER_SCL, EXTENDER_SPEED);
  touchIoex.attach(Wire);
  touchIoex.polarity(PCA95x5::Polarity::ORIGINAL_ALL);

  // Same reset setup used in the official SenseCAP Arduino touch example.
  touchIoex.write(EXPANDER_IO_LCD_RESET, PCA95x5::Level::L);
  touchIoex.direction(EXPANDER_IO_LCD_RESET, PCA95x5::Direction::OUT);

  touchIoex.direction(EXPANDER_IO_LCD_INT, PCA95x5::Direction::IN);

  touchIoex.write(EXPANDER_IO_TP_RESET, PCA95x5::Level::L);
  touchIoex.direction(EXPANDER_IO_TP_RESET, PCA95x5::Direction::OUT);



  touchIoex.write(EXPANDER_IO_RADIO_NSS, PCA95x5::Level::H);
  touchIoex.direction(EXPANDER_IO_RADIO_NSS, PCA95x5::Direction::OUT);
  touchIoex.direction(EXPANDER_IO_RADIO_RST, PCA95x5::Direction::OUT);
  touchIoex.direction(EXPANDER_IO_RADIO_BUSY, PCA95x5::Direction::IN);
  touchIoex.direction(EXPANDER_IO_RADIO_DIO_1, PCA95x5::Direction::IN);

  delay(5);
  touchIoex.write(EXPANDER_IO_LCD_RESET, PCA95x5::Level::H);
  touchIoex.write(EXPANDER_IO_TP_RESET, PCA95x5::Level::H);
  delay(10);

  extenderInitDone = true;
}

void initTouch() {
  extender_init_once();
  touch.init();
  touchReady = true;
  Serial.println("[touch] TouchLib FT5x06 init done at 0x48");
}

void mapTouchToScreen(int16_t rawX, int16_t rawY, int &x, int &y) {
  int tx = rawX;
  int ty = rawY;

  if (TOUCH_SWAP_XY) {
    int t = tx;
    tx = ty;
    ty = t;
  }

  if (TOUCH_INVERT_X) tx = 479 - tx;
  if (TOUCH_INVERT_Y) ty = 479 - ty;

  x = constrain(tx, 0, 479);
  y = constrain(ty, 0, 479);
}

bool readTouchPoint(int &x, int &y) {
  if (!touchReady) return false;
  if (!touch.read()) return false;

  uint8_t n = touch.getPointNum();
  if (n == 0) return false;

  TP_Point p = touch.getPoint(0);
  mapTouchToScreen(p.x, p.y, x, y);
  return true;
}

// =======================
// HELPERS
// =======================
String shortText(String s, int maxLen) {
  s.trim();
  if (s.length() <= maxLen) return s;
  return s.substring(0, maxLen - 1) + ".";
}

void swapItems(int a, int b) {
  char tempName[NAME_LEN];
  strncpy(tempName, itemNames[a], NAME_LEN);
  tempName[NAME_LEN - 1] = '\0';

  float tempDays = itemDaysLeft[a];

  strncpy(itemNames[a], itemNames[b], NAME_LEN);
  itemNames[a][NAME_LEN - 1] = '\0';
  itemDaysLeft[a] = itemDaysLeft[b];

  strncpy(itemNames[b], tempName, NAME_LEN);
  itemNames[b][NAME_LEN - 1] = '\0';
  itemDaysLeft[b] = tempDays;
}

String inventorySignature() {
  String sig = String(itemCount) + "|";
  for (int i = 0; i < itemCount; i++) {
    sig += String(itemNames[i]);
    sig += ":";
    sig += String(itemDaysLeft[i], 1);
    sig += ";";
  }
  return sig;
}

bool sensorFresh() {
  if (!hasSensorEverWorked) return false;
  return (millis() - lastGoodSensorMs) <= SENSOR_STALE_AFTER;
}

String sensorSignature() {
  return String(lightValue) + "|" + String(tempC, 1) + "|" + lightStatus + "|" + tempStatus + "|" + String(dataOk) + "|" + sensorDebug;
}

void clearItems() {
  itemCount = 0;
  for (int i = 0; i < MAX_ITEMS; i++) {
    itemNames[i][0] = '\0';
    itemDaysLeft[i] = 99.0;
  }
}

void calculateScroll() {
  int contentHeight = itemCount * FOOD_ROW_HEIGHT;
  maxScroll = contentHeight - FOOD_VIEW_HEIGHT;

  if (maxScroll < 0) maxScroll = 0;

  if (scrollFloat > maxScroll) scrollFloat = maxScroll;
  if (scrollFloat < 0) scrollFloat = 0;

  scrollOffset = (int)(scrollFloat + 0.5f);
  if (scrollOffset > maxScroll) scrollOffset = maxScroll;
  if (scrollOffset < 0) scrollOffset = 0;
}

void sortInventoryByExpiry() {
  for (int i = 0; i < itemCount - 1; i++) {
    for (int j = i + 1; j < itemCount; j++) {
      if (itemDaysLeft[j] < itemDaysLeft[i]) {
        swapItems(i, j);
      }
    }
  }
}

// =======================
// WIFI
// =======================
void connectWiFiBlocking() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(250);
    tries++;
  }

  wifiOk = (WiFi.status() == WL_CONNECTED);

  Serial.print("[wifi] ");
  if (wifiOk) {
    Serial.print("connected ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("failed");
  }
}

void maybeReconnectWiFi() {
  unsigned long now = millis();

  if (WiFi.status() == WL_CONNECTED) {
    wifiOk = true;
    return;
  }

  wifiOk = false;
  dataOk = false;
  invOk = false;

  if (now - lastReconnectTry < RECONNECT_INTERVAL) return;
  lastReconnectTry = now;

  Serial.println("[wifi] reconnecting...");
  WiFi.disconnect(false);
  WiFi.begin(ssid, password);
}

// =======================
// FETCH SENSOR DATA
// =======================
void markSensorFail(const String &reason, int code) {
  sensorFailCount++;
  lastSensorHttpCode = code;
  sensorDebug = reason;

  // Do NOT instantly blank the temp/light after one weak request.
  // Keep the last good reading for a bit so the UI does not flicker offline.
  if (!sensorFresh() || sensorFailCount >= 4) {
    dataOk = false;
  }

  Serial.print("[sensor] fail ");
  Serial.print(sensorFailCount);
  // Serial.print(" | ");
  Serial.print(reason);
  Serial.print(" | code=");
  Serial.println(code);
}

bool fetchSensorData() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiOk = false;
    markSensorFail("wifi disconnected", 0);
    return false;
  }

  wifiOk = true;

  WiFiClient client;
  HTTPClient http;

  // Important: the Arduino Uno R4 web server sometimes takes longer than 350-500 ms.
  // A too-short timeout is why the display said "No Arduino Data" even when Arduino worked.
  http.setTimeout(ARDUINO_HTTP_TIMEOUT);
  http.setReuse(false);
  client.setTimeout(ARDUINO_HTTP_TIMEOUT);

  if (!http.begin(client, dataUrl)) {
    markSensorFail("bad Arduino URL", -1000);
    client.stop();
    return false;
  }

  int code = http.GET();

  if (code != HTTP_CODE_OK) {
    http.end();
    client.stop();
    markSensorFail("http error", code);
    return false;
  }

  String payload = http.getString();
  http.end();
  client.stop();
  payload.trim();

  if (payload.length() == 0) {
    markSensorFail("empty response", -1001);
    return false;
  }

  StaticJsonDocument<768> doc;
  DeserializationError err = deserializeJson(doc, payload);

  if (err) {
    markSensorFail("json failed", -1002);
    Serial.print("[sensor] payload was: ");
    Serial.println(payload.substring(0, 180));
    return false;
  }

  lightValue = doc["light"] | lightValue;
  tempC = doc["tempC"] | tempC;
  tempF = doc["tempF"] | tempF;

  const char* ls = doc["lightStatus"] | lightStatus.c_str();
  const char* ts = doc["tempStatus"] | tempStatus.c_str();

  lightStatus = String(ls);
  tempStatus  = String(ts);

  doorOpen = (lightStatus == "BRIGHT" || lightStatus == "VERY BRIGHT");

  hasSensorEverWorked = true;
  lastGoodSensorMs = millis();
  sensorFailCount = 0;
  lastSensorHttpCode = 200;
  sensorDebug = "OK";
  dataOk = true;

  String sig = sensorSignature();
  bool changed = (sig != lastSensorSignature);
  if (changed) lastSensorSignature = sig;

  // Serial.print("[sensor] OK ");
  // Serial.print(lightValue);
  Serial.print(" | ");
  // Serial.print(tempC, 1);
  // Serial.print("C | ");
  // Serial.println(lightStatus);

  return changed;
}

// =======================
// FETCH INVENTORY
// =======================
bool fetchInventory() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiOk = false;
    invOk = false;
    return false;
  }

  wifiOk = true;

  WiFiClient client;
  HTTPClient http;

  http.setTimeout(450);
  http.setReuse(false);

  if (!http.begin(client, invUrl)) {
    invOk = false;
    return false;
  }

  int code = http.GET();

  if (code != 200) {
    http.end();
    invOk = false;
    Serial.print("[inventory] HTTP ");
    Serial.println(code);
    return false;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, payload);

  if (err) {
    invOk = false;
    Serial.println("[inventory] JSON failed");
    return false;
  }

  clearItems();

  JsonArray arr;

  if (doc.is<JsonArray>()) {
    arr = doc.as<JsonArray>();
  } else if (doc["items"].is<JsonArray>()) {
    arr = doc["items"].as<JsonArray>();
  } else if (doc["inventory"].is<JsonArray>()) {
    arr = doc["inventory"].as<JsonArray>();
  } else if (doc["foods"].is<JsonArray>()) {
    arr = doc["foods"].as<JsonArray>();
  }

  if (arr.isNull()) {
    invOk = false;
    Serial.println("[inventory] no array found");
    return false;
  }

  for (JsonObject obj : arr) {
    if (itemCount >= MAX_ITEMS) break;

    const char* nm = obj["name"] | obj["item"] | obj["food"] | "item";

    float dl = 99.0;
    if (obj["daysLeft"].is<float>() || obj["daysLeft"].is<int>()) {
      dl = obj["daysLeft"] | 99.0;
    } else if (obj["days_left"].is<float>() || obj["days_left"].is<int>()) {
      dl = obj["days_left"] | 99.0;
    } else if (obj["days"].is<float>() || obj["days"].is<int>()) {
      dl = obj["days"] | 99.0;
    }

    strncpy(itemNames[itemCount], nm, NAME_LEN - 1);
    itemNames[itemCount][NAME_LEN - 1] = '\0';

    itemDaysLeft[itemCount] = dl;
    itemCount++;
  }

  sortInventoryByExpiry();
  calculateScroll();

  invOk = true;

  String sig = inventorySignature();
  bool changed = (sig != lastInvSignature);
  if (changed) {
    lastInvSignature = sig;
    Serial.print("[inventory] updated items=");
    Serial.println(itemCount);
  }

  return changed;
}

// =======================
// DRAWING
// =======================
void drawBootScreen() {
  gfx->fillScreen(BG);

  gfx->setTextColor(GREEN);
  gfx->setTextSize(5);
  gfx->setCursor(55, 140);
  gfx->print("FridgeIQ");

  gfx->setTextColor(GRAY);
  gfx->setTextSize(2);
  gfx->setCursor(92, 220);
  gfx->print("Starting display");

  for (int i = 0; i < 8; i++) {
    uint16_t c = (i % 2 == 0) ? GREEN : AMBER;
    gfx->fillCircle(155 + i * 22, 285, 7, c);
    delay(50);
  }
}

void drawHeader(const char* subtitle, const char* modeText, uint16_t modeColor) {
  gfx->fillRect(0, 0, 480, 78, CARD);
  gfx->drawFastHLine(0, 78, 480, GREEN_DIM);

  gfx->setTextColor(GREEN);
  gfx->setTextSize(4);
  gfx->setCursor(16, 14);
  gfx->print("FridgeIQ");

  gfx->fillCircle(450, 22, 8, wifiOk ? GREEN : RED);

  gfx->setTextColor(modeColor);
  gfx->setTextSize(2);
  gfx->setCursor(330, 24);
  gfx->print(modeText);

  gfx->setTextColor(GRAY);
  gfx->setTextSize(1);
  gfx->setCursor(18, 60);
  gfx->print(subtitle);
}

void drawFooter(const char* text) {
  gfx->fillRect(0, 446, 480, 34, BG);
  gfx->drawFastHLine(0, 446, 480, DARKGRAY);

  gfx->setTextColor(DARKGRAY);
  gfx->setTextSize(1);
  gfx->setCursor(12, 458);
  gfx->print(text);

  gfx->setCursor(330, 458);
  gfx->print("wifi:");
  gfx->print(wifiOk ? "on " : "off");
  gfx->print(" pi:");
  gfx->print(invOk ? "on" : "off");
  gfx->print(" ard:");
  gfx->print(sensorFresh() ? "on" : "off");
}

void drawFoodFrame() {
  gfx->fillScreen(BG);
  drawHeader("Food expiry list - sorted by use first", "FOOD", AMBER);
  drawFooter("Drag list  |  swipe left/right");
}

void drawFoodRows() {
  // NO-CANVAS / NO-BLANK-SCREEN SCROLL:
  // Old version cleared the whole food area first, which made black flicker.
  // This version only repaints row cards + small gap strips, so the list does not flash black.

  if (itemCount == 0) {
    gfx->fillRect(0, FOOD_LIST_TOP - 4, 480, FOOD_VIEW_HEIGHT + 8, BG);

    gfx->fillRoundRect(32, 165, 416, 170, 18, CARD);
    gfx->drawRoundRect(32, 165, 416, 170, 18, DARKGRAY);

    gfx->setTextColor(GREEN);
    gfx->setTextSize(4);
    gfx->setCursor(95, 210);
    gfx->print("All fresh");

    gfx->setTextColor(GRAY);
    gfx->setTextSize(2);
    gfx->setCursor(72, 275);
    gfx->print(invOk ? "No items found" : "Waiting for Pi");
    return;
  }

  // Clean only the narrow safe background/gap areas, not the whole list.
  gfx->fillRect(0, FOOD_LIST_TOP - 4, 480, 8, BG);
  gfx->fillRect(0, FOOD_LIST_BOTTOM - 6, 480, 12, BG);
  gfx->fillRect(466, FOOD_LIST_TOP - 4, 14, FOOD_VIEW_HEIGHT + 8, BG);

  // Draw visible rows. Each row paints its own rounded rectangle over its old position.
  for (int i = 0; i < itemCount; i++) {
    int y = FOOD_LIST_TOP + i * FOOD_ROW_HEIGHT - scrollOffset;

    // Only draw full rows inside the visible window. This prevents header/footer damage.
    if (y < FOOD_LIST_TOP || y > FOOD_LIST_BOTTOM - 62) continue;

    bool expired = itemDaysLeft[i] <= 0;
    bool urgent = itemDaysLeft[i] > 0 && itemDaysLeft[i] <= 1.0;
    bool warning = itemDaysLeft[i] > 1.0 && itemDaysLeft[i] <= 2.0;

    uint16_t border = expired ? RED : (urgent ? RED : (warning ? AMBER : GREEN));
    uint16_t rowBg = expired ? 0x3000 : (urgent ? 0x3000 : (warning ? 0x2100 : CARD));

    // Clear just this row's card rectangle plus tiny padding, then draw the row.
    // This avoids a black full-area flash while still removing old text trails.
    gfx->fillRoundRect(14, y, 452, 60, 12, rowBg);
    gfx->drawRoundRect(14, y, 452, 60, 12, border);

    // Clear the 12px gap under each row only if it is safely inside the list.
    int gapY = y + 60;
    if (gapY >= FOOD_LIST_TOP && gapY < FOOD_LIST_BOTTOM) {
      int gapH = FOOD_ROW_HEIGHT - 60;
      if (gapY + gapH > FOOD_LIST_BOTTOM) gapH = FOOD_LIST_BOTTOM - gapY;
      if (gapH > 0) gfx->fillRect(0, gapY, 466, gapH, BG);
    }

    gfx->setTextColor(WHITE);
    gfx->setTextSize(3);
    gfx->setCursor(28, y + 15);

    String nm = shortText(String(itemNames[i]), 12);
    gfx->print(nm);

    gfx->setTextColor(border);
    gfx->setTextSize(2);

    String label;

    if (expired) {
      label = "EXPIRED";
    } else if (itemDaysLeft[i] < 1.0) {
      int hrs = (int)(itemDaysLeft[i] * 24.0);
      if (hrs < 1) hrs = 1;
      label = String(hrs) + "h left";
    } else if (itemDaysLeft[i] <= 1.05) {
      label = "1d left";
    } else {
      label = String(itemDaysLeft[i], 1) + "d left";
    }

    int lx = 456 - label.length() * 13;
    if (lx < 255) lx = 255;

    gfx->setCursor(lx, y + 22);
    gfx->print(label);
  }

  // Scroll bar: clear only the bar strip and redraw.
  if (maxScroll > 0) {
    gfx->fillRect(472, FOOD_LIST_TOP, 4, FOOD_VIEW_HEIGHT, DARKGRAY);
    int barH = 58;
    int barY = FOOD_LIST_TOP + (int)((scrollOffset * 1.0 / maxScroll) * (FOOD_VIEW_HEIGHT - barH));
    if (barY < FOOD_LIST_TOP) barY = FOOD_LIST_TOP;
    if (barY > FOOD_LIST_BOTTOM - barH) barY = FOOD_LIST_BOTTOM - barH;
    gfx->fillRect(472, barY, 4, barH, AMBER);
  }

  // Footer count area only; no full footer redraw.
  gfx->fillRect(0, 420, 170, 18, BG);
  gfx->setTextColor(DARKGRAY);
  gfx->setTextSize(1);
  gfx->setCursor(18, 425);
  gfx->print(String(itemCount) + " items");
}

void drawSensorFrame() {
  gfx->fillScreen(BG);
  drawHeader("Live Arduino fridge sensors", "LIVE", CYAN);

  gfx->fillRoundRect(18, 100, 444, 92, 16, CARD);
  gfx->fillRoundRect(18, 220, 212, 155, 16, CARD);
  gfx->drawRoundRect(18, 220, 212, 155, 16, DARKGRAY);

  gfx->fillRoundRect(250, 220, 212, 155, 16, CARD);
  gfx->drawRoundRect(250, 220, 212, 155, 16, DARKGRAY);

  gfx->setTextColor(GRAY);
  gfx->setTextSize(2);
  gfx->setCursor(40, 238);
  gfx->print("TEMP");

  gfx->setCursor(272, 238);
  gfx->print("LIGHT");

  drawFooter("Swipe left/right or tap header");
}

void drawSensorValues() {
  bool showSensor = sensorFresh();
  bool connecting = !hasSensorEverWorked;

  // Clear only dynamic areas instead of full-screen redraw.
  gfx->fillRect(22, 104, 436, 84, showSensor ? (doorOpen ? 0x2100 : 0x0120) : 0x3000);
  uint16_t doorColor = showSensor ? (doorOpen ? AMBER : GREEN) : RED;
  gfx->drawRoundRect(18, 100, 444, 92, 16, doorColor);

  gfx->setTextColor(doorColor);
  gfx->setTextSize(3);
  gfx->setCursor(42, 122);

  if (!showSensor) {
    gfx->print(connecting ? "CONNECTING..." : "ARDUINO OFFLINE");
  } else {
    gfx->print(doorOpen ? "DOOR OPEN" : "DOOR CLOSED");
  }

  gfx->setTextColor(GRAY);
  gfx->setTextSize(2);
  gfx->setCursor(42, 160);
  if (showSensor) {
    gfx->print(lightStatus);
  } else {
    String msg = sensorDebug;
    if (lastSensorHttpCode != 0) {
      msg += " ";
      msg += String(lastSensorHttpCode);
    }
    gfx->print(shortText(msg, 24));
  }

  gfx->fillRect(30, 276, 188, 86, CARD);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(5);
  gfx->setCursor(40, 280);
  if (showSensor) gfx->print(tempC, 1);
  else gfx->print("--");

  gfx->setTextColor(showSensor ? BLUE : RED);
  gfx->setTextSize(2);
  gfx->setCursor(40, 340);
  if (showSensor) gfx->print(tempStatus.substring(0, 12));
  else gfx->print(connecting ? "connecting" : "offline");

  gfx->fillRect(262, 276, 188, 86, CARD);
  gfx->setTextColor(doorColor);
  gfx->setTextSize(4);
  gfx->setCursor(272, 285);
  if (showSensor) gfx->print(lightValue);
  else gfx->print("--");

  gfx->setTextColor(doorColor);
  gfx->setTextSize(2);
  gfx->setCursor(272, 340);
  if (showSensor) gfx->print(lightStatus.substring(0, 13));
  else gfx->print("no /data");
}


void switchToFoodScreen() {
  currentScreen = SCREEN_FOOD;
  scrollOffset = 0;
  scrollFloat = 0.0f;
  screenStartTime = millis();

  // Inventory fetch happens on screen switch, not during scrolling.
  fetchInventory();
  calculateScroll();

  drawFoodFrame();
  drawFoodRows();

  lastScrollStep = millis();
}

void switchToSensorScreen() {
  currentScreen = SCREEN_SENSOR;
  screenStartTime = millis();

  fetchSensorData();

  drawSensorFrame();
  drawSensorValues();

  lastSensorFetch = millis();
}


void handleTouch() {
  int x, y;
  bool pressed = readTouchPoint(x, y);
  unsigned long now = millis();

  if (pressed) {
    lastUserTouchMs = now;

    if (!touchDown) {
      touchDown = true;
      touchStartX = x;
      touchStartY = y;
      touchLastX = x;
      touchLastY = y;
      touchPrevDragY = y;
      touchStartMs = now;
      lastTouchDrawMs = 0;
      return;
    }

    int dy = y - touchPrevDragY;
    touchLastX = x;
    touchLastY = y;

    if (currentScreen == SCREEN_FOOD && maxScroll > 0 && abs(dy) >= 2) {
      // Finger up moves the list down. Use a multiplier so it does not fly too fast.
      scrollFloat -= (float)dy * TOUCH_SCROLL_SCALE;
      if (scrollFloat < 0) scrollFloat = 0;
      if (scrollFloat > maxScroll) scrollFloat = maxScroll;

      int newOffset = (int)(scrollFloat + 0.5f);
      if (newOffset < 0) newOffset = 0;
      if (newOffset > maxScroll) newOffset = maxScroll;

      touchPrevDragY = y;

      if (newOffset != scrollOffset && now - lastTouchDrawMs >= TOUCH_DRAW_INTERVAL) {
        scrollOffset = newOffset;
        drawFoodRows();
        lastTouchDrawMs = now;
      }
    }

    return;
  }

  if (!touchDown) return;

  // Release event.
  touchDown = false;
  lastUserTouchMs = now;

  int dx = touchLastX - touchStartX;
  int dyTotal = touchLastY - touchStartY;

  bool isSwipeSide = abs(dx) >= SWIPE_MIN_DIST && abs(dx) > abs(dyTotal) * 1.2;
  bool isTap = abs(dx) <= TAP_MAX_MOVE && abs(dyTotal) <= TAP_MAX_MOVE && (now - touchStartMs) < 650;

  if (isSwipeSide) {
    if (currentScreen == SCREEN_FOOD) switchToSensorScreen();
    else switchToFoodScreen();
    return;
  }

  // Tap top header to swap screens. Easy backup if swipe feels weird.
  if (isTap && touchStartY < 85) {
    if (currentScreen == SCREEN_FOOD) switchToSensorScreen();
    else switchToFoodScreen();
    return;
  }

  if (currentScreen == SCREEN_FOOD) {
    scrollFloat = scrollOffset;
    drawFoodRows();
    lastScrollStep = now;
  }
}

void updateHeartbeat() {
  unsigned long now = millis();
  if (now - lastHeartbeat < HEARTBEAT_INTERVAL) return;

  lastHeartbeat = now;
  heartbeatPulse = !heartbeatPulse;

  uint16_t c;
  if (!wifiOk) c = RED;
  else c = heartbeatPulse ? GREEN : GREEN_DIM;

  gfx->fillCircle(450, 22, 8, c);
}

// =======================
// SETUP + LOOP
// =======================
void setup() {
  Serial.begin(115200);
  delay(200);

  extender_init_once();

  if (!gfx->begin()) {
    Serial.println("[display] gfx begin failed");
  }

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  initTouch();

  drawBootScreen();

  connectWiFiBlocking();

  fetchSensorData();
  fetchInventory();
  calculateScroll();

  lastSensorFetch = millis();
  lastInventoryFetch = millis();
  lastScrollStep = millis();
  screenStartTime = millis();

  drawFoodFrame();
  drawFoodRows();

  Serial.println("[ready] FridgeIQ display with official TouchLib running - orientation fixed");
}

void loop() {
  unsigned long now = millis();

  maybeReconnectWiFi();
  handleTouch();

  if (currentScreen == SCREEN_FOOD) {
    // Do NOT run HTTP requests here. This keeps scrolling smooth.
    bool touchPaused = (now - lastUserTouchMs) < TOUCH_AUTOSCROLL_PAUSE;

    if (maxScroll > 0) {
      if (!touchPaused && now - lastScrollStep >= SCROLL_INTERVAL) {
        lastScrollStep = now;
        scrollFloat += 1.0f;
        scrollOffset = (int)(scrollFloat + 0.5f);

        if (scrollOffset > maxScroll + 24) {
          switchToSensorScreen();
        } else {
          drawFoodRows();
        }
      }
    } else {
      if (!touchPaused && now - screenStartTime >= NO_SCROLL_FOOD_TIME) {
        switchToSensorScreen();
      }
    }
  }

  if (currentScreen == SCREEN_SENSOR) {
    if (now - lastSensorFetch >= SENSOR_INTERVAL) {
      lastSensorFetch = now;
      bool changed = fetchSensorData();

      // Redraw values on success, failure, or data change.
      if (changed || !dataOk) {
        drawSensorValues();
      }
    }

    // Inventory can update here because this screen is not scrolling.
    if (now - lastInventoryFetch >= INVENTORY_INTERVAL) {
      lastInventoryFetch = now;
      fetchInventory();
    }

    bool touchPaused = (now - lastUserTouchMs) < TOUCH_AUTOSCROLL_PAUSE;
    if (!touchPaused && now - screenStartTime >= SENSOR_SCREEN_TIME) {
      switchToFoodScreen();
    }
  }

  updateHeartbeat();

  delay(4);
}