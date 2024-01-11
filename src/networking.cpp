#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// CONSTANTS

const char* mqtt_client_name = "jelmerdejong";
const char* subscriptions[] = {};

// VARIABLES

WiFiClient wifiClient; // Initialize the wifi client
PubSubClient mqtt_client(wifiClient); // Initialize the mqtt client with the wifi client



void connectMqttClient() {
    Serial.println("Connecting to the mqtt server");
    while (!mqtt_client.connect(mqtt_client_name)) { // While not connected, keep trying to connect
        Serial.println("Cannot connect to the mqtt server, state=" + mqtt_client.state());
        delay(1000);
    }
    Serial.println("Connected to the mqtt server");
}

void messageCallback(char* topic, byte* payload, unsigned int length) {
    //TODO: Handle incoming messages to resume after reboot
};

void setupNetworking() { // Used to connect to the wifi network and mqtt server
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to the wifi network
    while (WiFi.status() != WL_CONNECTED) { // While not connected, keep trying to connect
        delay(1000);
        Serial.println("Cannot connect to the wifi network");
    }

    Serial.println("Connected to the wifi network");


    mqtt_client.setServer(MQTT_SERVER_IP, 1883); // Now set the mqtt server ip address and port
    mqtt_client.setCallback(messageCallback); // Set incoming message callback

    connectMqttClient(); // Try to connect

    for (const char * subscription : subscriptions) { // Subscribe to all topics in 'subscriptions'
        mqtt_client.subscribe(subscription);
    }
}

void publishData(const char* topic, const char* data, const unsigned int dataLength) {
    if (!mqtt_client.connected()) // If not connected anymore, first try to reconnect before publishing
        connectMqttClient();

    mqtt_client.beginPublish(topic, dataLength, true); // Begin publishing
    mqtt_client.write(reinterpret_cast<const uint8_t *>(data), dataLength); // reinterpret_cast voor const char* naar const byte*
    mqtt_client.endPublish(); // Send the message
}

void networkingLoop() {
    if (!mqtt_client.loop()) // Get incoming messages, and if not connected, reconnect
        connectMqttClient();
}