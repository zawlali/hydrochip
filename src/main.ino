#define BLYNK_TEMPLATE_ID "TMPL6gNly4l4U"
#define BLYNK_TEMPLATE_NAME "Hydrochip"
#define BLYNK_AUTH_TOKEN "5WwS6hOo26gXnQjY0qgfGXMzoiPKs6o4"

#define WF_SSID "OPPO_8E9986_2.4G"
#define PASS "UEPFE4Vd"
#define MOIST_PIN 36
#define HUMID_PIN 32
#define WATER_PIN 25
#define SPRAY_PIN 26
#define PUMP_PIN 27

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

// Global objects & variables
BlynkTimer timer;

DHT_Unified dht(HUMID_PIN, DHT11);

sensor_t dht_sensor;

//Irrigation
int dryness = 0;
int moisture = 0;
double moisture_pg = 0.0;
double moisture_thresh = 70.0;
int water_time = 6000;
bool water_manual = false;

//Spray
double humidity = 0.0;
double humidity_thresh = 70.0;
double humidity_pg = 0.0;
double temperature = 0.0;
int spray_time = 1000;
bool spray_manual = false;

void dataProc() {

  //Moisture sensor readings
  dryness = analogRead(MOIST_PIN);
  moisture = 4095 - dryness;
  moisture_pg = static_cast<double>(moisture) * 100 / 4095.0; 

  //DHT11 sensor readings [temp & humidity sensor]
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    temperature = event.temperature;
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    humidity = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("%"));
  } 

  Serial.print("MoisturePG: ");
  Serial.print(moisture_pg);
  Serial.println("%");
  Serial.println("--------------------------------");
}

void sendBlynk() {
  Blynk.virtualWrite(V0, dryness);
  Blynk.virtualWrite(V1, moisture);
  Blynk.virtualWrite(V2, moisture_pg);
  Blynk.virtualWrite(V9, humidity);
  Blynk.virtualWrite(V10, temperature);

  Serial.println("SENT TO BLYNK!!");
}

void waterPlants() {
  if (moisture_pg < moisture_thresh || water_manual) {
    digitalWrite(WATER_PIN, HIGH);  
    digitalWrite(PUMP_PIN, HIGH);
    Serial.println("Watering plants!");
    Blynk.virtualWrite(V4, true);
    delay(water_time);  // Water for 3 seconds
    digitalWrite(WATER_PIN, LOW);   
    digitalWrite(PUMP_PIN, LOW);
    Blynk.virtualWrite(V4, false);
    Serial.println("Watering complete!");
    if (water_manual) water_manual = false;
  }
}

void sprayPlants() {
  if (humidity_pg < humidity_thresh || spray_manual) {
    digitalWrite(SPRAY_PIN, HIGH);
    digitalWrite(PUMP_PIN, HIGH);
    Serial.println("Spraying plants!");
    Blynk.virtualWrite(V8, true);
    delay(8000); //replace this
    digitalWrite(SPRAY_PIN, LOW);
    digitalWrite(PUMP_PIN, LOW);
    Blynk.virtualWrite(V8, false);
    Serial.println("Spraying complete!");
    if (spray_manual) spray_manual = false;
  }
}

//V3: Moisture threshold
BLYNK_WRITE(V3) {
  moisture_thresh = param.asDouble();
  Serial.print("New moisture threshold set to: ");
  Serial.println(moisture_thresh);
}

//V4: Trigger irrigation manually/Irrigation status
BLYNK_WRITE(V4) {
  water_manual = static_cast<bool>(param.asInt());
  Serial.print("Manual trigger received: ");
  Serial.println(water_manual);
  waterPlants();
}

//V5: Duration of irrigation
BLYNK_WRITE(V5) {
  water_time = param.asInt() * 1000;
  Serial.print("New water time set to: ");
  Serial.println(water_time);
}

//V6: Humidity threshold
BLYNK_WRITE(V6) {
  humidity_thresh = param.asInt();
  Serial.print("New humidity threshold set to: ");
  Serial.println(humidity_thresh);
}

//V7: Duration of spray
BLYNK_WRITE(V7) {
  spray_time = param.asInt() * 1000;
  Serial.print("New spray time set to: ");
  Serial.println(spray_time);
}

//V8: Trigger spray manually/Spray status
BLYNK_WRITE(V8) {
  spray_manual = static_cast<bool>(param.asInt());
  Serial.print("Manual trigger received: ");
  Serial.println(spray_manual);
  sprayPlants();
}

void setup() {
  // Debug console
  Serial.begin(115200);

  //Pin setup
  pinMode(MOIST_PIN, INPUT);
  pinMode(WATER_PIN, OUTPUT);
  pinMode(SPRAY_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

   // Connect to WiFi first
  WiFi.mode(WIFI_STA);
  WiFi.begin(WF_SSID, PASS);
  
  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi Connection Failed!");
    ESP.restart();
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address IS: ");
  Serial.println(WiFi.localIP());

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, WF_SSID, PASS);
  timer.setInterval(1500L, dataProc);
  timer.setInterval(60000L, sendBlynk);
  timer.setInterval(300000L, waterPlants);

  //OTA setup
  ArduinoOTA.setHostname("hydrochip");
  ArduinoOTA.setPort(3232); 

  ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else  // U_SPIFFS
                type = "filesystem";
            Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

    ArduinoOTA.begin();
    Serial.println("OTA Ready");

    //DHT11 sensor initialization
    dht.begin();

    //Write to Blynk, default starting values
    Blynk.virtualWrite(V3, moisture_thresh);
    Blynk.virtualWrite(V5, water_time / 1000);
    Blynk.virtualWrite(V6, humidity_thresh);
    Blynk.virtualWrite(V7, spray_time / 1000);
}

void loop() {
  Blynk.run();
  timer.run();
  ArduinoOTA.handle();
}