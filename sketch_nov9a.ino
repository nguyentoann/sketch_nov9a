#include <Arduino.h>
#include "DHT.h"
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

// Thresholds (defaults: tune as needed)
int mqThreshold[3] = { 300, 300, 600 };  // MQ ADC thresholds (MQ1->index0 = GPIO32, MQ2->index1=33, MQ3->index2=34)
float dhtTempThreshold = 30.0;           // °C
float dustThreshold = 50.0;              // µg/m³ (or same unit you compute)

unsigned long lastSerialCheck = 0;
const unsigned long SERIAL_CHECK_INTERVAL = 200;  // ms



DHT dht(DHTPIN, DHTTYPE);



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


void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 Sensor Reading Start");

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



  delay(2000);  // wait 2 seconds between readings
}
