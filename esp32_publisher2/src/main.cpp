#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

/* ---------- WIFI ---------- */
const char* WIFI_SSID = "Mișu";
const char* WIFI_PASSWORD = "galacticEA2002";

/* ---------- MQTT ---------- */
const char* MQTT_BROKER_HOSTNAME = "mihai-raspberry.local";
const int MQTT_PORT = 1883;

const char* MQTT_USER = "espuser";
const char* MQTT_PASSWORD = "1234";

/* ---------- DEVICE ---------- */
const char* DEVICE_ID = "esp32-01";
const char* MQTT_TOPIC = "esp32/esp32-01/data";

/* ---------- CLIENTS ---------- */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/* ---------- CONTROL ---------- */
unsigned long lastPublish = 0;
const long PUBLISH_INTERVAL_MS = 1000;

/* ---------- RESOLVE mDNS ---------- */
String resolveMDNS(const char* hostname) {
  // Strip .local suffix for MDNS.queryHost()
  String host = String(hostname);
  host.replace(".local", "");

  Serial.print("Resolving ");
  Serial.print(hostname);
  Serial.print("... ");

  IPAddress ip = MDNS.queryHost(host, 3000); // 3 second timeout

  if (ip == IPAddress(0, 0, 0, 0)) {
    Serial.println("failed! Will retry...");
    return "";
  }

  Serial.println(ip.toString());
  return ip.toString();
}

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

  // Start mDNS after WiFi is connected
  if (!MDNS.begin(DEVICE_ID)) {
    Serial.println("mDNS init failed");
  } else {
    Serial.println("mDNS started");
  }
}

/* ---------- MQTT CONNECT ---------- */
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");

    // Resolve hostname fresh each attempt in case IP changed
    String brokerIP = resolveMDNS(MQTT_BROKER_HOSTNAME);
    if (brokerIP == "") {
      Serial.println("Cannot resolve broker, retrying in 3s...");
      delay(3000);
      continue;
    }

    mqttClient.setServer(brokerIP.c_str(), MQTT_PORT);

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
  connectMQTT();
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

    float temp = temperatureRead();

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