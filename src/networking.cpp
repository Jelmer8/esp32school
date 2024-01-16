#include "doorfeature.h"
#include "secrets.h"
#include <PubSubClient.h>
#include <WiFi.h>

#include <movementfeature.h>

using namespace std;

// CONSTANTS

const char* mqtt_client_name = "jelmerdejong";
const char* subscriptions[] = {"jelmerdejong/doorOpenTimestamp", "jelmerdejong/doorCurrentlyOpen", "jelmerdejong/lastMotionTimestamp"};

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

void messageCallback(const char* topic, const byte* payload, unsigned int length) { // Incoming message callback
    Serial.println(topic);

    char* newPayload = static_cast<char*>(malloc(length + 1)); // Allocate new char array
    strncpy(newPayload, reinterpret_cast<const char*>(payload), length); // Copy payload into the new array
    newPayload[length] = '\0'; // Needed because payload does not yet contain the null character


    Serial.println(newPayload);


    if (topic == "jelmerdejong/doorOpenTimestamp") {
        resumeDoorTimestamp(stoi(newPayload));
    }

    if (topic == "jelmerdejong/doorCurrentlyOpen") {
        resumeDoorState(stoi(newPayload));
    }

    if (topic == "jelmerdejong/lastMotionTimestamp") {
        resumeMotionTimestamp(stoi(newPayload));
    }

    free(newPayload); // Free allocated memory
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
    mqtt_client.print(data); // Send this data
    mqtt_client.endPublish(); // Send the message
}

void networkingLoop() {
    if (!mqtt_client.loop()) // Get incoming messages, and if not connected, reconnect
        connectMqttClient();
}