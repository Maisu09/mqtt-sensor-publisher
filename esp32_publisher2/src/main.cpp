#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* ---------- WIFI ---------- */
const char* WIFI_SSID     = "Mihai";
const char* WIFI_PASSWORD = "galacticEA2002";

/* ---------- MQTT ---------- */
const char* MQTT_SERVER   = "192.168.0.244";
const int   MQTT_PORT     = 1883;
const char* MQTT_USER     = "espuser";
const char* MQTT_PASSWORD = "1234";

/* ---------- DEVICE ---------- */
const char* DEVICE_ID   = "esp32-02";
const char* MQTT_TOPIC  = "esp32/esp32-02/data";

/* ---------- SENSOR ---------- */
#define PUBLISH_INTERVAL 1000

WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastPublish = 0;

/* -------------------------------------------------- */
void connectWiFi() {
  Serial.printf("\nConnecting to WiFi: %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected — IP: %s\n", WiFi.localIP().toString().c_str());
}

/* -------------------------------------------------- */
void connectMQTT() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  String clientId = String(DEVICE_ID) + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  while (!mqttClient.connected()) {
    Serial.printf("Connecting to MQTT broker %s:%d ...", MQTT_SERVER, MQTT_PORT);
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(" connected!");
    } else {
      Serial.printf(" failed (state=%d), retrying in 5s\n", mqttClient.state());
      delay(5000);
    }
  }
}

void publishTemperature() {
  Serial.println("publishTemperature() called");
  
  float tempC = temperatureRead();
  Serial.printf("Raw tempC: %.2f\n", tempC);
  
  // If temperatureRead() returns 0 or an implausible value, skip
  if (tempC < 0 || tempC > 150) {
    Serial.println("Bad temperature reading, skipping.");
    return;
  }

  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  StaticJsonDocument<128> doc;
  doc["device"] = DEVICE_ID;
  doc["tempC"]  = serialized(String(tempC, 2));
  doc["tempF"]  = serialized(String(tempF, 2));

  char payload[128];
  serializeJson(doc, payload);

  if (mqttClient.publish(MQTT_TOPIC, payload, false)) {
    Serial.printf("Published → %s : %s\n", MQTT_TOPIC, payload);
  } else {
    Serial.println("Publish FAILED.");
  }
}

/* -------------------------------------------------- */
void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectMQTT();
}

/* -------------------------------------------------- */
void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;
    publishTemperature();
  }
}