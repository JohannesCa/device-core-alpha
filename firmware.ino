#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>

#define BUZZER D2
#define LOCK D7


/*********************************************/
/* Initial Config                            */
/*********************************************/

/* Device config */
const char* device_id = "41e9c21f-0552-4535-b7c5-9ac7c3fa36fd";
const char* device_name = "esp8266-home-market-controller";

/* Wifi config */
const char* ssid = "";
const char* password = "";

/* Server config */
const char* server_host = "";
const int server_port = 8080;

/* NTP Time Servers */
const char* ntp1 = "time.windows.com";
const char* ntp2 = "pool.ntp.org";
time_t now;


/*********************************************/
/* Task Scheduler Setup                      */
/*********************************************/

Scheduler taskScheduler;

bool lockIsOpen = false;

void healthcheckCallback();
void wsclientCallback();
void openLockCallback();
void closeLockCallback();

Task healthcheckTask(10 * TASK_SECOND, TASK_FOREVER, &healthcheckCallback, &taskScheduler);
Task wsclientTask(500 * TASK_MILLISECOND, TASK_FOREVER, &wsclientCallback, &taskScheduler);
Task lockTask(5 * TASK_SECOND, TASK_FOREVER, &openLockCallback, &taskScheduler);


/*********************************************/
/* Web Socket Setup                          */
/*********************************************/

websockets::WebsocketsClient wsclient;

void onMessageCallback(websockets::WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());

  JsonDocument doc;
  deserializeJson(doc, message.data());

  if (doc["event"] == "registration") {
    if (doc["data"] != "ok") {
      Serial.println("Received invalid registration response. Resetting...");
      ESP.reset();
    }

  } else if (doc["event"] == "command") {
    if (doc["data"] == "open") {
      Serial.println("Received \"open\" request. openning lock...");

      lockTask.abort();
      lockTask.setCallback(&openLockCallback);
      lockTask.enable();

      DynamicJsonDocument resDoc(128);
      resDoc["event"] = "command";
      resDoc["timestamp"] = now;
      resDoc["data"] = "ok";

      char buff[128];
      serializeJson(resDoc, buff);
      Serial.printf("sending response to command open: %s\n", buff);
      wsclient.send(buff);
    }
  }
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
  if (event == websockets::WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
    ESP.reset();
  } else if (event == websockets::WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == websockets::WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

// Setup wsclient
void setupWebSocket() {
  // Add callback methods
  wsclient.onMessage(onMessageCallback);
  wsclient.onEvent(onEventsCallback);

  wsclient.connect(server_host, server_port, "/device/connect");

  int tries = 0;
  while (!wsclient.available(true)) {
    Serial.print(".");
    delay(1000);

    if (tries++ == 9) {
      Serial.println("\nFailed to connect to ws server. Rebooting...");
      ESP.reset();
    }
  }

  // Prepare registration data
  char regData[250];
  sprintf(regData, "{\"device_id\":\"%s\",\"device_name\":\"%s\"}", device_id, device_name);

  // Prepare Json event
  DynamicJsonDocument doc(256);
  doc["event"] = "registration";
  doc["timestamp"] = now;
  doc["data"] = regData;

  char buff[256];
  serializeJson(doc, buff);

  Serial.printf("sending data: \n%s\n", buff);
  wsclient.send(buff);
}


/*********************************************/
/* Task Methods                              */
/*********************************************/

// Health check
void healthcheckCallback() {
  //Serial.println("healthcheck task: Executing...");
  time(&now);

  DynamicJsonDocument heartbeatDoc(64);
  heartbeatDoc["event"] = "heartbeat";
  heartbeatDoc["timestamp"] = now;
  heartbeatDoc["device_id"] = device_id;
  heartbeatDoc["data"] = "healthy";

  char buff[250];
  serializeJson(heartbeatDoc, buff);

  wsclient.send(buff);
}

// WebSocket pool
void wsclientCallback() {
  //Serial.println("ws task: Executing...");
  wsclient.poll();
}

void openLockCallback() {
  lockTask.setCallback(&closeLockCallback);

  if (lockIsOpen) {
    return;
  }

  tone(BUZZER, 880, 150);

  digitalWrite(LOCK, HIGH);
  lockIsOpen = true;
}

void closeLockCallback() {
  tone(BUZZER, 659, 100);
  delay(100);

  digitalWrite(LOCK, LOW);
  lockIsOpen = false;

  lockTask.setCallback(&openLockCallback);
  lockTask.disable();
}

/*********************************************/
/* Service Functions                         */
/*********************************************/

void beepBoot() {
  tone(BUZZER, 659, 100);
  delay(150);
  tone(BUZZER, 880, 150);
  delay(150);
}

void beepStep() {
  tone(BUZZER, 880, 100);
  delay(100);
}

void beepBootComplete() {
  tone(BUZZER, 880, 200);
  delay(200);
}


/*********************************************/
/* ESP Setup                                 */
/*********************************************/

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK, OUTPUT);

  beepBoot();

  // Connect to wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait until we are connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nSuccessfully connected to WiFi, setting time... ");

  // We configure ESP8266's time, as we need it to validate the certificates
  configTime(-3 * 3600, 1, ntp1, ntp2);
  while (now < 2 * 3600) {
    Serial.print(".");
    delay(500);
    now = time(nullptr);
  }
  Serial.println("\nTime set, connecting to server...");

  // Prepare websocket connection
  setupWebSocket();

  wsclientTask.enable();
  Serial.println("Enabled websocket task");

  healthcheckTask.enable();
  Serial.println("Enabled healthcheck task");

  Serial.println("Successfully booted!");
  beepBootComplete();
}


/*********************************************/
/* Main Loop                                 */
/*********************************************/

void loop() {
  taskScheduler.execute();
}
