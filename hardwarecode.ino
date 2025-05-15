#define BLYNK_TEMPLATE_ID "TMPL27zkAqT_W"
#define BLYNK_TEMPLATE_NAME "ECG System"
#define BLYNK_AUTH_TOKEN "Q7BmAHBYW2zKZzr_JA_c8GMxew7r0S5p"

#define BLYNK_PRINT Serial

const char* WIFI_SSID = "Hayoy <3";
const char* WIFI_PASS = "hayaa2006";

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h" 
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SERVO_PIN 13
#define VP_BPM 0
#define VP_SPO2 1
#define VP_ACTIVITY 2
#define VP_AQI 3
#define VP_MED_COMMAND 4
#define VP_ALERT_STATUS 5
#define READINGS_SAMPLE_SIZE 100
#define ACTIVITY_THRESHOLD 150
#define REST_CONFIRM_SAMPLES 50
#define BPM_HIGH_THRESHOLD 100
#define BPM_LOW_THRESHOLD 50
#define SPO2_LOW_THRESHOLD 90
#define SPO2_MODERATE_THRESHOLD 86
#define AQI_POOR_THRESHOLD 100
#define AQI_VERY_POOR_THRESHOLD 150

const char* AQI_API_KEY = "YOUR_AQI_API_KEY";
const char* AQI_CITY = "YourCity";
String aqiApiUrl = "https://api.waqi.info/feed/" + String(AQI_CITY) + "/?token=" + String(AQI_API_KEY);

PulseOximeter pox; 

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Servo medServo;
BlynkTimer timer;

float currentBPM = 0;
int currentSPO2 = 0;
int currentAQI = -1;
String activityState = "Initializing";
String alertMessage = "Nominal";
float lastAccelMag = 0;
int stableSampleCount = 0;

void readSensors();
void checkActivity();
void fetchAQI_API();
void checkAlerts();
void sendDataToBlynk();
BLYNK_WRITE(VP_MED_COMMAND);

// --- Setup Function ---
void setup() {
    Serial.begin(115200);
    // WAIT FOR SERIAL TO BE READY - IMPORTANT FOR SOME ESP32 BOARDS
    // ESPECIALLY IF YOU OPEN SERIAL MONITOR RIGHT AFTER RESETTING
    delay(2000); // Wait 2 seconds for serial connection to establish
    Serial.println("HealthGuardian Booting Up... Stage 1: Serial Begun"); // <<< ADDED

    Wire.begin(); // Initialize I2C (SDA=21, SCL=22 are default for ESP32)
    Serial.println("Stage 2: Wire.begin() called"); // <<< ADDED

    Serial.print("Initializing pulse oximeter..");
    if (!pox.begin()) {
        Serial.println(" FAILED - MAX30100 pox.begin()"); // <<< MODIFIED
        while (1); // Halt on failure
    } else {
        Serial.println(" SUCCESS - MAX30100 pox.begin()"); // <<< MODIFIED
        // For "MAX30100lib by OXullo Intersecans", you often set the mode
        // pox.setMode(PulseOximeter::MODE_BOTH); // Try a constant from the library's .h if MAX30100_MODE_SPO2_HR fails
        // Serial.println("Stage 3a: pox.setMode() attempted (if uncommented)"); // <<< ADDED (if you uncomment setMode)
    }
    Serial.println("Stage 3: After MAX30100 Init attempt"); // <<< ADDED

    Serial.print("Initializing ADXL345...");
    if (!accel.begin()) {
        Serial.println(" FAILED - ADXL345 accel.begin()"); // <<< MODIFIED
        while (1); // Halt on failure
    } else {
        Serial.println(" SUCCESS - ADXL345 accel.begin()"); // <<< MODIFIED
    }
    Serial.println("Stage 4: After ADXL345 Init attempt"); // <<< ADDED

    // Only set range if accel.begin() was successful
    if (accel.begin()) { // Redundant check, but safe
        accel.setRange(ADXL345_RANGE_2_G);
        Serial.println("Stage 4a: ADXL345 range set"); // <<< ADDED
    }


    medServo.attach(SERVO_PIN);
    medServo.write(0);
    Serial.println("Stage 5: Servo Attached"); // <<< ADDED

    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
    Serial.println("Stage 6: After Blynk.begin() called"); // <<< ADDED


    // Check Blynk connection explicitly after begin
    if (Blynk.connected()) {
        Serial.println("Stage 6a: Confirmed CONNECTED to Blynk!");
    } else {
        Serial.println("Stage 6b: FAILED to connect to Blynk after begin call.");
        // You might want to add a loop here to retry Blynk connection or halt
    }

    timer.setInterval(1000L, readSensors);
    Serial.println("Stage 7: readSensors timer set"); // <<< ADDED
    timer.setInterval(500L, checkActivity);
    Serial.println("Stage 8: checkActivity timer set"); // <<< ADDED

    // Temporarily comment out AQI to isolate other issues
    // timer.setInterval(300000L, fetchAQI_API);
    // Serial.println("Stage 9: fetchAQI_API timer set (but commented out)"); // <<< ADDED

    timer.setInterval(5000L, checkAlerts);
    Serial.println("Stage 10: checkAlerts timer set"); // <<< ADDED
    timer.setInterval(2000L, sendDataToBlynk);
    Serial.println("Stage 11: sendDataToBlynk timer set"); // <<< ADDED

    // fetchAQI_API(); // Temporarily commented out
    Serial.println("Stage 12: Setup Complete. Entering loop()."); // <<< ADDED
}

void loop() {
    pox.update(); // Make sure to call update as fast as possible
    Blynk.run();
    timer.run();
}

// --- Sensor Reading Function ---
void readSensors() {
   
    float tempBPM = pox.getHeartRate(); // Function name matches example
    int tempSPO2 = pox.getSpO2();     // Function name matches example

    if (tempBPM > 20 && tempBPM < 250) { // A beat was detected and is plausible
        currentBPM = tempBPM;
        if (tempSPO2 > 50 && tempSPO2 <= 100) { // SpO2 is also plausible
            currentSPO2 = tempSPO2;
        } else {
            currentSPO2 = 0; // SpO2 invalid, but BPM might be okay
        }
    } else {
        // No valid beat detected, or BPM out of range
        currentBPM = 0;
        currentSPO2 = 0;
    }
    
}

// --- Activity Checking Function (Remains the same) ---
void checkActivity() {
    sensors_event_t event;
    accel.getEvent(&event);
    float accelMag = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
    float deltaMag = abs(accelMag - lastAccelMag);

    if (deltaMag < ACTIVITY_THRESHOLD) {
        stableSampleCount++;
    } else {
        stableSampleCount = 0;
        activityState = "Active";
    }

    if (stableSampleCount > REST_CONFIRM_SAMPLES) {
        activityState = "Resting";
        stableSampleCount = REST_CONFIRM_SAMPLES + 1;
    }
    lastAccelMag = accelMag;
}

// --- Fetch AQI from API Function (Remains the same) ---
void fetchAQI_API() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        Serial.print("Fetching AQI from: "); Serial.println(aqiApiUrl);
        http.begin(aqiApiUrl);
        int httpCode = http.GET();

        if (httpCode > 0) {
            String payload = http.getString();
            DynamicJsonDocument doc(1024); 
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.print("deserializeJson() failed: "); Serial.println(error.c_str());
                currentAQI = -2;
            } else {
                if (doc["status"] == "ok" && doc["data"]["aqi"].is<int>()) { 
                   currentAQI = doc["data"]["aqi"];
                   Serial.print("AQI Fetched: "); Serial.println(currentAQI);
                } else {
                   Serial.println("API returned error status or AQI not found/not int.");
                   currentAQI = -3;
                }
            }
        } else {
            Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
            currentAQI = -4;
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected, cannot fetch AQI.");
        currentAQI = -5;
    }
}

// --- Alert Checking Function (Remains the same, logic for BPM/SpO2 values is now more reliable) ---
void checkAlerts() {
    bool physiologicalAlert = false;
    //bool environmentalRisk = (currentAQI >= AQI_POOR_THRESHOLD && currentAQI >= 0);
    //bool highEnvironmentalRisk = (currentAQI >= AQI_VERY_POOR_THRESHOLD && currentAQI >= 0);
    bool environmentalRisk = false; // Assume no environmental risk for now
    bool highEnvironmentalRisk = false; // Assume no high environmental risk for now
    alertMessage = "Nominal";

    if (activityState == "Resting" && (currentBPM > 0 || currentSPO2 > 0)) { // Check if readings are considered valid
        if (currentSPO2 < SPO2_LOW_THRESHOLD && currentSPO2 > 0) { // currentSPO2 > 0 implies valid
            alertMessage = "CRITICAL: Low SpO2!";
            Blynk.logEvent("low_spo2_alert", alertMessage);
            physiologicalAlert = true;
        } else if ((currentBPM > BPM_HIGH_THRESHOLD || currentBPM < BPM_LOW_THRESHOLD) && currentBPM > 0) { // currentBPM > 0 implies valid
            alertMessage = "CRITICAL: Abnormal Heart Rate!";
            Blynk.logEvent("abnormal_hr_alert", alertMessage);
            physiologicalAlert = true;
        }
    }

    if (!physiologicalAlert) {
        if (activityState == "Resting" && environmentalRisk && currentSPO2 > 0 && currentSPO2 < SPO2_LOW_THRESHOLD + 5) {
           alertMessage = "WARNING: Poor AQI & Borderline SpO2!";
           Blynk.logEvent("contextual_spo2_alert", alertMessage);
        } else if (highEnvironmentalRisk) {
           alertMessage = "INFO: Very Poor Air Quality Today!";
           Blynk.logEvent("high_aqi_info", alertMessage);
        } else if (environmentalRisk) {
           alertMessage = "INFO: Poor Air Quality Today.";
        }
    }
    Blynk.virtualWrite(VP_ALERT_STATUS, alertMessage);
}

// --- Send Data to Blynk Function (Remains the same) ---
void sendDataToBlynk() {
    if (Blynk.connected()) {
        Blynk.virtualWrite(VP_BPM, currentBPM);
        Blynk.virtualWrite(VP_SPO2, currentSPO2);
        Blynk.virtualWrite(VP_ACTIVITY, activityState);
        //Blynk.virtualWrite(VP_AQI, currentAQI);
    }
}

// --- Blynk Medication Command Handler (Remains the same) ---
BLYNK_WRITE(VP_MED_COMMAND) {
    int command = param.asInt();
    Serial.print("Medication Command Received: "); Serial.println(command);
    switch (command) {
        case 0: medServo.write(0); break;
        case 1: medServo.write(70); break;
        case 2: medServo.write(130); break;
        case 3: medServo.write(180); break;
        default: medServo.write(0); break;
    }
}
