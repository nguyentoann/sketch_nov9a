#include <Arduino.h>
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DUST_LED_PIN 12
#define DUST_VOUT_PIN 35

#define DHTPIN 13      // GPIO 13 for DHT sensor
#define DHTTYPE DHT11  // or DHT11 depending on your sensor

#define MQ1_PIN 32  // MQ sensor 1
#define MQ2_PIN 33  // MQ sensor 2
#define MQ3_PIN 34  // MQ sensor 3

// === Fan & Buzzer ===
#define FAN_PIN 18     // active LOW
#define BUZZER_PIN 19  // active LOW

// WiFi Configuration
const char* WIFI_SSID = "demo273";  // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "pass1234";  // Replace with your WiFi password

// Server Configuration
// Public server URL (with domain name)
const char* PUBLIC_SERVER_URL = "https://iotproject001.toandz.id.vn";  // Replace with your public server domain
// Local server will use default gateway IP with port 8085
const int LOCAL_SERVER_PORT = 8085;

// Server state tracking
enum ServerState {
  SERVER_PUBLIC,   // Using public server
  SERVER_LOCAL,    // Using local server
  SERVER_UNKNOWN   // Not determined yet
};

ServerState currentServer = SERVER_UNKNOWN;  // Start with unknown, will try public first
const int HTTP_SUCCESS = 200;

// Thresholds (defaults: tune as needed)
int mqThreshold[3] = { 300, 300, 600 };  // MQ ADC thresholds (MQ1->index0 = GPIO32, MQ2->index1=33, MQ3->index2=34)
float dhtTempThreshold = 30.0;           // °C
float dustThreshold = 50.0;              // µg/m³ (or same unit you compute)

unsigned long lastSerialCheck = 0;
const unsigned long SERIAL_CHECK_INTERVAL = 200;  // ms

unsigned long lastDataPost = 0;
unsigned long lastPoll = 0;
const unsigned long DATA_POST_INTERVAL = 2000;  // 2 seconds
const unsigned long POLL_INTERVAL = 2000;        // 2 seconds

unsigned long startTime = 0;  // For uptime calculation

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
HTTPClient http;



void handleSerialCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;
  cmd.toLowerCase();

  // tokenize (up to 4 tokens)
  String parts[4];
  int p = 0;
  char buf[cmd.length() + 1];
  cmd.toCharArray(buf, sizeof(buf));
  char *tok = strtok(buf, " ");
  while (tok && p < 4) {
    parts[p++] = String(tok);
    tok = strtok(NULL, " ");
  }

  // mq commands: "mq1 set 350" or "mq1 350"
  if (parts[0].startsWith("mq")) {
    int idx = parts[0].substring(2).toInt() - 1;  // mq1 -> index 0
    if (idx < 0 || idx > 2) {
      Serial.println("Invalid mq index (use mq1/mq2/mq3).");
      return;
    }
    String last = parts[p - 1];
    int val = last.toInt();
    mqThreshold[idx] = val;
    Serial.print("MQ");
    Serial.print(idx + 1);
    Serial.print(" threshold set to ");
    Serial.println(val);
    return;
  }

  // dht commands: "dht set temp 30" or "dht temp 30" or "dht 30"
  if (parts[0] == "dht") {
    String last = parts[p - 1];
    float val = last.toFloat();
    dhtTempThreshold = val;
    Serial.print("DHT temperature threshold set to ");
    Serial.println(val);
    return;
  }

  // dust commands: "dust set 80" or "dust 80"
  if (parts[0] == "dust") {
    String last = parts[p - 1];
    float val = last.toFloat();
    dustThreshold = val;
    Serial.print("Dust threshold set to ");
    Serial.println(val);
    return;
  }

  // status
  if (parts[0] == "status") {
    Serial.println("=== Thresholds ===");
    Serial.print("MQ1: ");
    Serial.println(mqThreshold[0]);
    Serial.print("MQ2: ");
    Serial.println(mqThreshold[1]);
    Serial.print("MQ3: ");
    Serial.println(mqThreshold[2]);
    Serial.print("DHT Temp: ");
    Serial.println(dhtTempThreshold);
    Serial.print("Dust: ");
    Serial.println(dustThreshold);
    return;
  }

  Serial.println("Unknown command. Examples:");
  Serial.println("  mq1 set 350");
  Serial.println("  dht 30");
  Serial.println("  dust 50");
  Serial.println("  status");
}


String getPublicServerURL() {
  return String(PUBLIC_SERVER_URL);
}

String getLocalServerURL() {
  // Use default gateway as server with port 8085
  IPAddress gateway = WiFi.gatewayIP();
  return "http://" + String(gateway[0]) + "." + String(gateway[1]) + "." + String(gateway[2]) + "." + String(gateway[3]) + ":" + String(LOCAL_SERVER_PORT);
}

String getCurrentServerURL() {
  if (currentServer == SERVER_LOCAL) {
    return getLocalServerURL();
  } else {
    return getPublicServerURL();
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Public server: ");
    Serial.println(getPublicServerURL());
    Serial.print("Local server: ");
    Serial.println(getLocalServerURL());
    Serial.println("Server selection: Will try public first, then local if public fails");
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
  }
}

bool tryPostToServer(String serverURL, String jsonPayload) {
  String url = serverURL + "/api/data";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);  // 5 second timeout
  
  int httpResponseCode = http.POST(jsonPayload);
  bool success = (httpResponseCode == HTTP_SUCCESS);
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.print(httpResponseCode);
    Serial.print(" from ");
    Serial.println(serverURL);
    if (httpResponseCode == HTTP_SUCCESS) {
      String response = http.getString();
      Serial.println("Response: " + response);
    }
  } else {
    Serial.print("Error code: ");
    Serial.print(httpResponseCode);
    Serial.print(" from ");
    Serial.println(serverURL);
  }
  
  http.end();
  return success;
}

void postSensorData(float temperature, float humidity, int mq1, int mq2, int mq3, float dust) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping data post");
    return;
  }
  
  // Create JSON payload
  StaticJsonDocument<512> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["mq1"] = mq1;
  doc["mq2"] = mq2;
  doc["mq3"] = mq3;
  doc["dust"] = dust;
  doc["wifiSignal"] = WiFi.RSSI();
  doc["uptime"] = (millis() - startTime) / 1000;  // uptime in seconds
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  Serial.print("Posting data... Payload: ");
  Serial.println(jsonPayload);
  
  // Server switching logic:
  // 1. If current server is unknown, try public first
  // 2. Try current server first
  // 3. If current server fails, try the other server
  // 4. If other server succeeds, switch to it
  // 5. If both fail, try public server again next time
  
  bool success = false;
  
  if (currentServer == SERVER_UNKNOWN || currentServer == SERVER_PUBLIC) {
    // Try public server first
    Serial.println("Trying public server...");
    success = tryPostToServer(getPublicServerURL(), jsonPayload);
    if (success) {
      currentServer = SERVER_PUBLIC;
      Serial.println("✓ Using public server");
    } else {
      // Public failed, try local server
      Serial.println("Public server failed, trying local server...");
      success = tryPostToServer(getLocalServerURL(), jsonPayload);
      if (success) {
        currentServer = SERVER_LOCAL;
        Serial.println("✓ Switched to local server");
      } else {
        Serial.println("✗ Both servers failed, will try public again next time");
        currentServer = SERVER_PUBLIC;  // Reset to try public first next time
      }
    }
  } else if (currentServer == SERVER_LOCAL) {
    // Try local server first
    Serial.println("Trying local server...");
    success = tryPostToServer(getLocalServerURL(), jsonPayload);
    if (success) {
      Serial.println("✓ Using local server");
    } else {
      // Local failed, try public server
      Serial.println("Local server failed, trying public server...");
      success = tryPostToServer(getPublicServerURL(), jsonPayload);
      if (success) {
        currentServer = SERVER_PUBLIC;
        Serial.println("✓ Switched to public server");
      } else {
        Serial.println("✗ Both servers failed, will try public again next time");
        currentServer = SERVER_PUBLIC;  // Reset to try public first next time
      }
    }
  }
}

bool tryPollFromServer(String serverURL) {
  String url = serverURL + "/api/poll";
  http.begin(client, url);
  http.setTimeout(5000);  // 5 second timeout
  
  int httpResponseCode = http.GET();
  bool success = (httpResponseCode == HTTP_SUCCESS);
  
  if (success) {
    String response = http.getString();
    Serial.print("Poll response from ");
    Serial.print(serverURL);
    Serial.print(": ");
    Serial.println(response);
    
    // Parse JSON response
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error && doc["update"].as<bool>()) {
      // Update thresholds from server
      if (doc.containsKey("thresholds")) {
        JsonObject thresholds = doc["thresholds"];
        
        if (thresholds.containsKey("mq1")) {
          mqThreshold[0] = thresholds["mq1"].as<int>();
          Serial.print("MQ1 threshold updated to: ");
          Serial.println(mqThreshold[0]);
        }
        if (thresholds.containsKey("mq2")) {
          mqThreshold[1] = thresholds["mq2"].as<int>();
          Serial.print("MQ2 threshold updated to: ");
          Serial.println(mqThreshold[1]);
        }
        if (thresholds.containsKey("mq3")) {
          mqThreshold[2] = thresholds["mq3"].as<int>();
          Serial.print("MQ3 threshold updated to: ");
          Serial.println(mqThreshold[2]);
        }
        if (thresholds.containsKey("temperature")) {
          dhtTempThreshold = thresholds["temperature"].as<float>();
          Serial.print("Temperature threshold updated to: ");
          Serial.println(dhtTempThreshold);
        }
        if (thresholds.containsKey("dust")) {
          dustThreshold = thresholds["dust"].as<float>();
          Serial.print("Dust threshold updated to: ");
          Serial.println(dustThreshold);
        }
      }
    }
  } else {
    Serial.print("Poll error code: ");
    Serial.print(httpResponseCode);
    Serial.print(" from ");
    Serial.println(serverURL);
  }
  
  http.end();
  return success;
}

void pollServer() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // Use the same server switching logic as postSensorData
  bool success = false;
  
  if (currentServer == SERVER_UNKNOWN || currentServer == SERVER_PUBLIC) {
    // Try public server first
    success = tryPollFromServer(getPublicServerURL());
    if (success) {
      currentServer = SERVER_PUBLIC;
    } else {
      // Public failed, try local server
      success = tryPollFromServer(getLocalServerURL());
      if (success) {
        currentServer = SERVER_LOCAL;
        Serial.println("✓ Switched to local server for polling");
      } else {
        currentServer = SERVER_PUBLIC;  // Reset to try public first next time
      }
    }
  } else if (currentServer == SERVER_LOCAL) {
    // Try local server first
    success = tryPollFromServer(getLocalServerURL());
    if (success) {
      // Local server working
    } else {
      // Local failed, try public server
      success = tryPollFromServer(getPublicServerURL());
      if (success) {
        currentServer = SERVER_PUBLIC;
        Serial.println("✓ Switched to public server for polling");
      } else {
        currentServer = SERVER_PUBLIC;  // Reset to try public first next time
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 Sensor Reading Start");
  
  startTime = millis();

  dht.begin();

  // Initialize analog pins (input by default)
  pinMode(MQ1_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ3_PIN, INPUT);
  pinMode(DUST_LED_PIN, OUTPUT);
  pinMode(DUST_VOUT_PIN, INPUT);

  // Fan & buzzer pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  // inactive = HIGH
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  
  // Connect to WiFi
  connectWiFi();
}

void loop() {


  // periodically check Serial (non-blocking-ish)
  if (millis() - lastSerialCheck > SERIAL_CHECK_INTERVAL) {
    lastSerialCheck = millis();
    while (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      handleSerialCommand(line);
    }
  }


  // Read DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("DHT -> Temp: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Read MQ sensors (0–4095 range on ESP32)
  int mq1_value = analogRead(MQ1_PIN);
  int mq2_value = analogRead(MQ2_PIN);
  int mq3_value = analogRead(MQ3_PIN);

  Serial.print("MQ1 (GPIO32): ");
  Serial.print(mq1_value);
  Serial.print(" | MQ2 (GPIO33): ");
  Serial.print(mq2_value);
  Serial.print(" | MQ3 (GPIO34): ");
  Serial.println(mq3_value);

  Serial.println("----------------------");


  // === Dust Sensor (Sharp GP2Y10) ===
  digitalWrite(DUST_LED_PIN, LOW);  // turn on IR LED
  delayMicroseconds(280);
  int dustVal = analogRead(DUST_VOUT_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);  // turn off LED
  delayMicroseconds(9680);           // finish 10ms cycle

  float voltage = dustVal * (3.3 / 4095.0);
  float dustDensity = (voltage - 0.9) * 1000 / 0.5;  // ≈ µg/m³
  if (dustDensity < 0) dustDensity = 0;

  Serial.print("Dust -> ADC: ");
  Serial.print(dustVal);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | Dust Density: ");
  Serial.print(dustDensity, 1);
  Serial.println(" µg/m³");






  // === Evaluate alerts ===
  // MQ alerts
  bool mqAlert = false;
  int mqVals[3] = { mq1_value, mq2_value, mq3_value };
  for (int i = 0; i < 3; ++i) {
    if (mqVals[i] >= mqThreshold[i]) {
      mqAlert = true;
      break;
    }
  }

  // DHT temp alert
  bool dhtTempAlert = (temperature >= dhtTempThreshold);

  // Dust alert
  bool dustAlert = (dustDensity >= dustThreshold);

  // Decide outputs:
  // Fan ON if (mqAlert && dhtTempAlert) OR dustAlert
  if (mqAlert || dhtTempAlert || dustAlert) {
    digitalWrite(FAN_PIN, LOW);  // active LOW -> turn on
  } else {
    digitalWrite(FAN_PIN, HIGH);  // off
  }

  // Buzzer ON only when mqAlert && dhtTempAlert
  if (mqAlert || dhtTempAlert) {
    digitalWrite(BUZZER_PIN, LOW);  // on
  } else {
    digitalWrite(BUZZER_PIN, HIGH);  // off
  }

  // Optional: print concise status
  Serial.print("ALERTs -> MQ:");
  Serial.print(mqAlert ? "1" : "0");
  Serial.print(" DHTtemp:");
  Serial.print(dhtTempAlert ? "1" : "0");
  Serial.print(" Dust:");
  Serial.print(dustAlert ? "1" : "0");
  Serial.print(" | Fan:");
  Serial.print(((digitalRead(FAN_PIN) == LOW) ? "ON" : "OFF"));
  Serial.print(" Buzzer:");
  Serial.println(((digitalRead(BUZZER_PIN) == LOW) ? "ON" : "OFF"));

  Serial.print("Check: MQ=");
  Serial.print(mqAlert);
  Serial.print(" DHTtemp=");
  Serial.print(dhtTempAlert);
  Serial.print(" Dust=");
  Serial.println(dustAlert);



  // Post data to server every 2 seconds
  if (millis() - lastDataPost >= DATA_POST_INTERVAL) {
    lastDataPost = millis();
    if (!isnan(temperature) && !isnan(humidity)) {
      postSensorData(temperature, humidity, mq1_value, mq2_value, mq3_value, dustDensity);
    }
  }
  
  // Poll server for threshold updates every 2 seconds
  if (millis() - lastPoll >= POLL_INTERVAL) {
    lastPoll = millis();
    pollServer();
  }
  
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting reconnect...");
    connectWiFi();
  }

  delay(1000);  // Small delay to prevent watchdog issues
}
