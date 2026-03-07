# mqtt-sensor-publisher

compose.yaml file contains the necessary configuration for the mosquitto service.
Created a publisher subscriber using mosquitto_sub and mosquitto_pub from the mosquitto-clients package.
sudo apt install mosquitto-clients

For creating a user it is necessary to install the full mosquitto package to have access to the mosquitto_passwd utility.
After creating a user pointing it to the pwfile the mosquitto service can be dissable so it does not interfier with the defaul port.
sudo apt install mosquitto
mosquitto_passwd /path/mosquitto/config/pwfile <user>
sudo systemctl stop mosquitto
sudo systemctl disable mosquitto

in mqtt-sensor-publiher main folder: docker compose -f docker-compose.docker.yaml up

For publish
mosquitto_sub -h 192.168.0.x:1883 -t "esp32/+/data" -u <user> -P <password>
mosquitto_pub -h 192.168.0.x -p 1883 -u <> -P <> -t "esp32/esp32-01/data" -m '{}'
mosquitto_pub -h 192.168.0.x -p 1883 -u <> -P <> -t "esp32/esp32-02/data" -m '{}'

mosquitto_sub -h localhost:8080 -t esp -u <user> -P <password>
mosquitto_pub -h localhost -t <topic> -m "messge" -u <user> -P <password>

Using PubSubClient for communication on MQTT from eps32 to RPI.
ESP publishing data to the mosquitto bronker running in a container on the RPI.


For prometheus access: http://192.168.0.x:9090

