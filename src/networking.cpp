#include <PubSubClient.h>
#include <WiFi.h>
#include "doorfeature.h"
#include "secrets.h"
#include "movementfeature.h"

using namespace std;

// CONSTANTS

const char* MQTT_CLIENT_NAME = "jelmerdejong";
const char* SUBSCRIPTIONS[] = {"jelmerdejong/doorOpenMsAgo", "jelmerdejong/doorCurrentlyOpen", "jelmerdejong/lastMotionMsAgo"};

// VARIABLES

WiFiClient wifiClient; // Initialize the wifi client
PubSubClient mqttClient(wifiClient); // Initialize the mqtt client with the wifi client



void connectMqttClient() {
    Serial.println("Connecting to the mqtt server");
    while (!mqttClient.connect(MQTT_CLIENT_NAME)) { // While not connected, keep trying to connect
        Serial.println("Cannot connect to the mqtt server, state=" + mqttClient.state());
        delay(1000);
    }
    Serial.println("Connected to the mqtt server");
}

void messageCallback(const char* topic, const byte* payload, const unsigned int length) { // Incoming message callback
    char* newPayload = static_cast<char*>(malloc(length + 1)); // Allocate new char array
    strncpy(newPayload, reinterpret_cast<const char*>(payload), length); // Copy payload into the new array
    newPayload[length] = '\0'; // Needed because payload does not yet contain the null character


    // See which topic the received message belongs to and run the correct function
    if (strcmp(topic, "jelmerdejong/doorOpenMsAgo") == 0) {
        resumeDoorMsAgo(stoi(newPayload));
    }

    if (strcmp(topic, "jelmerdejong/doorCurrentlyOpen") == 0) {
        resumeDoorState(stoi(newPayload));
    }

    if (strcmp(topic, "jelmerdejong/lastMotionMsAgo") == 0) {
        resumeMotionMsAgo(stoi(newPayload));
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


    mqttClient.setServer(MQTT_SERVER_IP, 1883); // Now set the mqtt server ip address and port
    mqttClient.setCallback(messageCallback); // Set incoming message callback

    connectMqttClient(); // Try to connect

    for (const char * subscription : SUBSCRIPTIONS) { // Subscribe to all topics in 'SUBSCRIPTIONS'
        mqttClient.subscribe(subscription);
    }
}

void publishData(const char* topic, const char* data, const unsigned int dataLength) {
    if (!mqttClient.connected()) // If not connected anymore, first try to reconnect before publishing
        connectMqttClient();

    mqttClient.beginPublish(topic, dataLength, true); // Begin publishing
    mqttClient.print(data); // Send this data
    mqttClient.endPublish(); // Send the message
}

void networkingLoop() {
    if (!mqttClient.loop()) // Get incoming messages, and if not connected, reconnect
        connectMqttClient();
}