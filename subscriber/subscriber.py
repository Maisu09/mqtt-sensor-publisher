import os
import time
import paho.mqtt.client as mqtt

BROKER_HOST = os.getenv("MQTT_HOST", "192.168.0.244")
BROKER_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "esp32/+/data")
MQTT_USER = os.getenv("MQTT_USER", "espuser")
MQTT_PASS = os.getenv("MQTT_PASS", "1234")
CLIENT_ID = os.getenv("MQTT_CLIENT_ID", "rpi-subscriber")

def on_connect(client, userdata, flags, reason_code, properties=None):
    print(f"[mqtt] connected with reason_code={reason_code}", flush=True)

    if reason_code == 0:
        result, mid = client.subscribe(MQTT_TOPIC)
        print(f"[mqtt] subscribed to topic: {MQTT_TOPIC}, result={result}, mid={mid}", flush=True)
    else:
        print("[mqtt] connection refused by broker", flush=True)

def on_message(client, userdata, msg):
    now = time.time()
    payload = msg.payload.decode("utf-8", errors="replace")
    print(f"[{now:.3f}] topic={msg.topic} payload={payload}", flush=True)

def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=CLIENT_ID)
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_message = on_message

    print(f"[mqtt] connecting to {BROKER_HOST}:{BROKER_PORT} ...", flush=True)
    client.connect(BROKER_HOST, BROKER_PORT, keepalive=60)
    client.loop_forever()

if __name__ == "__main__":
    main()
