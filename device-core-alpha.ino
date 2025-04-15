#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "src/App.h"

#define BUZZER D6
#define LOCK D7

App app(LOCK, BUZZER);

const char* ssid = "";
const char* password = "";
const char* websockets_connection_string = "ws://192.168.3.103/echo";

/* NTP Time Servers */
const char *ntp1 = "time.windows.com";
const char *ntp2 = "pool.ntp.org";
time_t now;

websockets::WebsocketsClient client;


void onMessageCallback(websockets::WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    if(event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
        ESP.reset();
    } else if(event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}


void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    Serial.println("Connecting to WiFi...");

    // Wait until we are connected to WiFi
    while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }

    Serial.println("Successfully connected to WiFi, setting time... ");

    // We configure ESP8266's time, as we need it to validate the certificates
    configTime(-3 * 3600, 1, ntp1, ntp2);
    while(now < 2 * 3600) {
        Serial.print(".");
        delay(500);
        now = time(nullptr);
    }
    Serial.println("");
    Serial.println("Time set, connecting to server...");

    // run callback when messages are received
    client.onMessage(onMessageCallback);

    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    // Before connecting, set the ssl certificates and key of the server
    // X509List cert(ca_cert);
    // client.setTrustAnchors(&cert);

    // X509List *serverCertList = new X509List(client_cert);
    // PrivateKey *serverPrivKey = new PrivateKey(client_private_key);
    // client.setClientRSACert(serverCertList, serverPrivKey);

    // Connect to server
    client.connect("192.168.3.103", 8080, "/echo");

    // Send a message
    // client.send("Hello Server");

    // Send a ping
    client.ping();

    DynamicJsonDocument doc(256);
    JsonObject root = doc.to<JsonObject>();
    root["event"] = "registration";
    root["timestamp"] = now;

    JsonObject eventData = root["data"].to<JsonObject>();
    eventData["device_id"] = "41e9c21f-0552-4535-b7c5-9ac7c3fa36fd";
    eventData["device_name"] = "esp8266-home-market-controller";

    JsonObject location = eventData["location"].to<JsonObject>();
    location["name"] = "";
    location["lat"] = "";
    location["long"] = "";

    char buff[256];

    serializeJson(doc, buff);

    Serial.printf("data: \n%s\n", buff);
    client.send(buff);

    Serial.printf("Staring app...");
    app.Setup();
}

void loop() {
    client.poll();

    tone(BUZZER, 440, 300);
    delay(600);
}
