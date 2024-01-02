#include <WiFi.h>
#include <PubSubClient.h>

#define BUTTON_PIN 26

//BUTTON
int lastState = HIGH; // the previous state from the input pin
int currentState; // the current reading from the input pin
int reading;

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 100; // the debounce time; increase if the output flickers


//MQTT/INTERNET
const char *ssid = "Wifi-56-78";
const char *password = "Va8rt14O#";

const char *mqtt_server = "192.168.68.68";

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println(".");
    }
}

void callback(char *topic, byte *message, unsigned int length) {
    // calback function for arrived message
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    Serial.write(message, length);
    Serial.println();
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Proberen te verbinden met MQTT server... ");
        // Attempt to connect
        if (client.connect("Jelmer")) {
            Serial.println("Verbonden");
            // Subscribe
            client.subscribe("esp32/output");
        } else {
            Serial.print("failed, state=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


void publishData(char *channel, char *data) {
    if (!client.connected())
        reconnect();

    client.publish(channel, data);
}


void readButtonState() {
    // read the state of the switch/button:
    reading = digitalRead(BUTTON_PIN);

    if (lastState != reading) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentState) {
            currentState = reading;
            if (currentState == LOW) {
                //button is pressed
                publishData("esp32/output", "owajdioajd989");
            }
        }
    }

    // save the last state
    lastState = reading;
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    readButtonState(); //update button state


    if (!client.connected())
        reconnect();
    client.loop();
    // for publish
    // client.publish("esp32/humidity", humString);
}
