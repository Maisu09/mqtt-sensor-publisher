#include <WiFi.h>
#include <PubSubClient.h>

/* ---------- WIFI ---------- */
const char* WIFI_SSID = "Mihai";
const char* WIFI_PASSWORD = "galacticEA2002";

/* ---------- MQTT ---------- */
const char* MQTT_SERVER = "192.168.0.244";
const int MQTT_PORT = 1883;

const char* MQTT_USER = "espuser";
const char* MQTT_PASSWORD = "1234";

/* ---------- DEVICE ---------- */
const char* DEVICE_ID = "esp32-02";
const char* MQTT_TOPIC = "esp32/esp32-02/data";

/* ---------- CLIENTS ---------- */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/* ---------- CONTROL ---------- */
unsigned long lastPublish = 0;
const long PUBLISH_INTERVAL_MS = 1000;

/* ---------- WIFI CONNECT ---------- */
void connectWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

/* ---------- MQTT CONNECT ---------- */
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");

    if (mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying...");
      delay(2000);
    }
  }
}

/* ---------- SETUP ---------- */
void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

/* ---------- LOOP ---------- */
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  unsigned long now = millis();

  if (now - lastPublish >= PUBLISH_INTERVAL_MS) {
    lastPublish = now;

    float temp = temperatureRead();  // internal ESP32 sensor

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"device\":\"%s\",\"temperature\":%.2f,\"ts_ms\":%lu}",
             DEVICE_ID, temp, now);

    bool ok = mqttClient.publish(MQTT_TOPIC, payload);

    Serial.print("Publish: ");
    Serial.print(payload);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAIL");
  }
}