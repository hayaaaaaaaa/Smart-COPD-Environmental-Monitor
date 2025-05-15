# HealthGuardian: Smart COPD & Environmental Monitor (SolveThe17 Hackathon)

## Project Overview
HealthGuardian is an innovative IoT wearable system designed to empower individuals with Chronic Obstructive Pulmonary Disease (COPD) by providing real-time physiological monitoring integrated with environmental air quality data and an ML-powered risk assessment. This project directly addresses SDG 3 (Good Health and Well-being) by enhancing patient care and SDG 11 (Sustainable Cities and Communities) by highlighting the impact of urban air quality on respiratory health.

## SDGs Addressed
*   **SDG 3: Good Health and Well-being:** Provides continuous remote monitoring for COPD patients, enabling timely alerts for potential exacerbations and facilitating proactive doctor-patient communication.
*   **SDG 11: Sustainable Cities and Communities:** Integrates local Air Quality Index (AQI) data to provide context to physiological readings, raising awareness about environmental pollution's impact on health in urban settings and contributing to smarter health management within cities.

## Features
*   Real-time monitoring of Heart Rate (BPM) and Blood Oxygen Saturation (SpO2) using MAX30100.
*   Activity detection (Resting/Active) using ADXL345.
*   Integration of real-time local Air Quality Index (AQI) via a web API.
*   ESP32-based wearable prototype.
*   Blynk IoT Platform for data visualization (web dashboard & mobile app) and alerts.
*   Simulated medication delivery via servo motor controlled through Blynk.
*   Contextual alerts based on combined physiological and AQI data.
*   **(If ML is implemented and working):** Machine Learning model ** (using Edge Impulse with simulated data) to classify exacerbation risk levels (Low, Medium, High, Critical) based on combined sensor inputs.

## Hardware Components
*   ESP32 DEVKITV1 (or ESP32 Dev Module)
*   MAX30100 Pulse Oximeter and Heart Rate Sensor
*   ADXL345 Accelerometer
*   Servo Motor (e.g., SG90 180-degree)
*   LED (for visual indication)
*   Water Pump (for simulated medication - optional for demo)
*   Relay Module (to control the pump - optional for demo)
*   Jumper Wires
*   Breadboard
*   (Optional for future: PMS5003 Air Quality Sensor)
*   Power source (USB from laptop for development, or LiPo battery + TP4056 for portability)

## Software & Libraries
*   Arduino IDE
*   ESP32 Arduino Core
*   **Libraries:**
    *   `WiFi.h`
    *   `WiFiClient.h`
    *   `BlynkSimpleEsp32.h`
    *   `Wire.h`
    *   `MAX30100_PulseOximeter.h` (from "MAX30100lib by OXullo Intersecans" - **Note:** `MAX30100_BeatDetector.cpp` within this library was modified to cast `BEATDETECTOR_MAX_THRESHOLD` to `(float)` in `min()` function calls to resolve type conflicts with ESP32 Core vX.X.X compiler.)
    *   `Adafruit_ADXL345_U.h`
    *   `Adafruit_Sensor.h`
    *   `ESP32Servo.h`
    *   `HTTPClient.h` (for AQI API)
    *   `ArduinoJson.h` (for AQI API)
    *   **(If ML is implemented):** The Edge Impulse exported Arduino library (`YourProjectName_inferencing.h`)
*   Blynk IoT Platform (Cloud, Web Dashboard, Mobile App)
*   Python 3 (for generating simulated ML data)
*   (Optional) Edge Impulse (for ML model development)

## Setup Instructions

### 1. Hardware Setup:
*   Connect the components as per the provided in the textual description below:
    *   I. Power Distribution (Using a Breadboard):
          Battery & Charger:
            *   Connect your Lithium Battery to a TP4056 charging module (B+ to battery +, B- to battery -).
            *   The TP4056 OUT+ will be your main 5V source. Connect it to the 5V power rail (+) of your breadboard.
            *   The TP4056 OUT- will be your main ground. Connect it to the ground rail (-) of your breadboard.
          ESP32 Power:
            *   Connect ESP32 VIN pin to the breadboard's 5V rail (+).
            *   Connect an ESP32 GND pin to the breadboard's ground rail (-).
          3.3V Rail (for 3.3V sensors):
            *   Connect the ESP32 3V3 pin to a separate 3.3V power rail (+) on your breadboard (if your breadboard has two power rails per side, use one for 5V and                   the other for 3.3V). If not, you'll connect 3.3V components directly from the ESP32 3V3 pin or use a dedicated row on the breadboard.
    *   II. I2C Devices (MAX30100 & ADXL345):
            *   I2C devices share the same data (SDA) and clock (SCL) lines.
          MAX30100:
            *   VIN (or VCC) -> Breadboard 3.3V Rail (+) (MAX30100 is usually 3.3V)
            *   GND -> Breadboard Ground Rail (-)
            *   SCL -> ESP32 GPIO 22 (Default I2C SCL)
            *   SDA -> ESP32 GPIO 21 (Default I2C SDA)
          ADXL345:
            *   VCC (or VIN) -> Breadboard 3.3V Rail (+) (ADXL345 is usually 3.3V)
            *   GND -> Breadboard Ground Rail (-)
            *   SCL -> ESP32 GPIO 22 (Shared with MAX30100)
            *   SDA -> ESP32 GPIO 21 (Shared with MAX30100)
            *   CS (Chip Select) -> Breadboard 3.3V Rail (+) (to enable I2C mode on most ADXL345 breakouts)
            *   SDO (Data Out / Address Select) -> Breadboard Ground Rail (-) (Often sets the I2C address to 0x53. If connected to 3.3V, it might be 0x1D. Check your                 breakout's datasheet.)
    *   III. Servo Motor:
            *   Signal Pin (usually Orange or Yellow) -> ESP32 GPIO 13 (or your chosen servo pin from the code)
            *   VCC / Power Pin (usually Red) -> Breadboard 5V Rail (+).
      Important: Do NOT power the servo directly from an ESP32 GPIO pin or the ESP32's 3.3V output. It needs more current. The 5V from the TP4056 output should be          sufficient.
            *   GND Pin (usually Brown or Black) -> Breadboard Ground Rail (-) (This ensures a common ground with the ESP32).
    *   IV. LED (Visual Alert):
            *   LED Anode (longer leg) -> A current-limiting Resistor (e.g., 220-330 Ohms).
            *   Other end of the Resistor -> ESP32 GPIO Pin (e.g., GPIO 2, GPIO 4, or any unused digital pin you define in your code for an alert LED).
            *   LED Cathode (shorter leg, flat side) -> Breadboard Ground Rail (-).
    *   V. Water Pump & Relay Module:
          The water pump likely requires more current/voltage than an ESP32 GPIO pin can safely provide. A relay acts as an electrically controlled switch.
          Relay Module Power:
            *   VCC -> Breadboard 5V Rail (+) (Most common relay modules for Arduino/ESP32 operate on 5V logic power).
            *   GND -> Breadboard Ground Rail (-).
            *   IN (or SIG) (Signal Pin) -> ESP32 GPIO Pin (e.g., GPIO 12, GPIO 14, or any unused digital pin you define in code to control the pump).
            *   Relay Switching Circuit (for the Pump):
          The relay will have three screw terminals (or pins) for the load:
            *   COM (Common)
            *   NO (Normally Open)
            *   NC (Normally Closed)
          Pump Power Source: The pump might need its own power source (e.g., another battery pack, a 5V or 6V supply) separate from the ESP32's logic power if it               draws significant current. Let's assume a separate power source for the pump for safety.
            *   Connect the Positive (+) terminal of the Pump's Power Source to one of the pump's wires.
            *   Connect the other wire of the Pump to the Relay's NO (Normally Open) terminal.
            *   Connect the Relay's COM (Common) terminal to the Negative (-) terminal of the Pump's Power Source.
Crucial (if using a separate power supply for the pump): Ensure the Ground (-) of the Pump's Power Source is ALSO connected to the ESP32's/Breadboard's main Ground Rail (-). This is essential for the relay control signal to work correctly.
How it works: When the ESP32 sends a HIGH signal to the relay's IN pin, the relay closes the connection between COM and NO, completing the circuit for the pump and turning it ON. When the signal is LOW, the connection opens, turning the pump OFF.
    *   VI. PMS5003 Air Quality Sensor (Future Addition - UART):
            *   VCC -> Breadboard 5V Rail (+) (PMS5003 is typically 5V logic and power).
            *   GND -> Breadboard Ground Rail (-).
            *   TXD (Sensor's Transmit Pin) -> ESP32 GPIO 16 (RX2) (ESP32's Second UART Receive Pin).
            *   RXD (Sensor's Receive Pin) -> ESP32 GPIO 17 (TX2) (ESP32's Second UART Transmit Pin).
    *   Ensure the ESP32 is powered via USB from your laptop.
    *   Make sure all I2C devices (MAX30100, ADXL345) share common SDA (GPIO21) and SCL (GPIO22) lines.
    *   Ensure all components share a common GND.
    *   MAX30100 and ADXL345 VCC to ESP32 3.3V.
    *   Servo VCC and Relay VCC to ESP32 VIN (5V from USB). Servo Signal to GPIO13. Relay IN to GPIO12 (example).

### 2. Arduino IDE Setup:
1.  Install the Arduino IDE (Version 1.8.18 or later recommended).
2.  Install the ESP32 board support package.
3.  Install the required libraries via the Arduino Library Manager (`Sketch > Include Library > Manage Libraries...`):
    *   `Blynk` by Volodymyr Shymanskyy
    *   `MAX30100lib` by OXullo Intersecans (**Important:** After installation, you may need to modify `MAX30100_BeatDetector.cpp` as noted in the Software section if you encounter type conflict errors with `min()`).
    *   `Adafruit ADXL345` by Adafruit
    *   `Adafruit Unified Sensor` by Adafruit
    *   `ESP32Servo` by Kevin Harrington / John K. Bennett
    *   `ArduinoJson` by Benoit Blanchon
    *   **(If ML is implemented):** Add the Edge Impulse exported library as a .ZIP library (`Sketch > Include Library > Add .ZIP Library...`).
4.  Open the `.ino` sketch file (`HealthGuardian.ino` or `hardwarecode.ino`).
5.  **Configure Credentials:**
    *   Update the Blynk credentials at the top of the sketch:
        ```cpp
        #define BLYNK_TEMPLATE_ID "TMPL27zkAqT_W" // Your actual Template ID
        #define BLYNK_TEMPLATE_NAME "ECG System"  // Your actual Template Name
        #define BLYNK_AUTH_TOKEN "Q7BmAHBYW2zK...S5p" // Your actual Auth Token
        ```
    *   Update your WiFi credentials:
        ```cpp
        const char* WIFI_SSID = "YourWiFiName";
        const char* WIFI_PASS = "YourWiFiPassword";
        ```
    *   Update your AQI API Key and City:
        ```cpp
        const char* AQI_API_KEY = "YOUR_ACTUAL_AQI_API_KEY";
        const char* AQI_CITY = "YourTargetCity"; // e.g., "cairo", "london", etc.
        ```
6.  Select the correct board in the Arduino IDE: `Tools > Board > ESP32 Arduino > ESP32 Dev Module`.
7.  Select the correct Port: `Tools > Port > COMx` (or `/dev/ttyUSBx` etc.).
8.  Upload the sketch to your ESP32.

### 3. Blynk Setup:
1.  Create a new Template in Blynk.Console (e.g., "ECG System" or "HealthGuardian").
2.  Define the following Datastreams:
    *   `V0`: Name: `BPM`, Type: `Float` or `Integer`, Units: `bpm`, Min: `0`, Max: `250`
    *   `V1`: Name: `SpO2`, Type: `Integer`, Units: `%`, Min: `0`, Max: `100`
    *   `V2`: Name: `Activity State`, Type: `String`
    *   `V3`: Name: `AQI`, Type: `Integer`, Min: `-10` (for errors), Max: `500`
    *   `V4`: Name: `Medication Command`, Type: `Integer`, Min: `0`, Max: `3`
    *   `V5`: Name: `Alert Status`, Type: `String`
    *   **(If ML is implemented):** `V6` (example): Name: `ML Risk Level`, Type: `String` or `Integer`
3.  Create a Device from this template to get your `BLYNK_AUTH_TOKEN`.
4.  Set up your Web Dashboard and Mobile Dashboard with widgets (Gauges, Charts, Value Displays, Buttons) linked to these datastreams.
    *   **Crucially, set appropriate Y-axis ranges for charts (e.g., BPM 0-250, SpO2 70-100).**
5.  Configure Events in the Template for notifications (e.g., for `low_spo2_alert`, `abnormal_hr_alert`).

### 4. (If ML is implemented) Machine Learning Model Setup (Simulated Data):
1.  Ensure Python 3 is installed.
2.  Run the `generate_copd_data.py` script to create `simulated_copd_data.csv`.
3.  Create an Edge Impulse account and a new project.
4.  Upload `simulated_copd_data.csv` to Edge Impulse:
    *   Label data using the `RiskLevel` column.
    *   Split data (e.g., 80% training / 20% testing).
5.  Create an Impulse:
    *   Input Block: Raw Data (select features: BPM, SpO2, Activity, AQI).
    *   Processing Block: Raw Data / Flatten.
    *   Learning Block: Classification (Keras).
6.  Generate features and train the classifier.
7.  Deploy as an Arduino library and add it to your Arduino IDE.
8.  Integrate the inference code into your main ESP32 sketch.

## How to Run
1.  Ensure all hardware is connected correctly and the ESP32 is powered via USB.
2.  Upload the configured Arduino sketch.
3.  Open the Arduino Serial Monitor (115200 baud) to observe debug messages and sensor readings.
4.  Open your Blynk Web Dashboard or Mobile App to see the data and control medication.
5.  Place a finger on the MAX30100 sensor for BPM/SpO2 readings.
6.  Move the ADXL345 to see changes in Activity State.
7.  AQI data should update periodically.
8.  Observe the ML Risk Level.

## Future Enhancements
*   Integration of a physical PMS5003 air quality sensor directly onto the wearable.
*   More sophisticated ML models (e.g., using LSTMs for time-series prediction).
*   Training ML models on real (anonymized) patient data (with ethical approval).
*   Miniaturization of the hardware into a more compact wearable.
*   Improved battery life and power management.
*   Direct integration with healthcare provider platforms.

## Contribution to SDGs
This project demonstrates a tangible way technology can be leveraged to improve individual health outcomes (SDG 3) while also considering and reacting to urban environmental factors (SDG 11), promoting a holistic approach to well-being in city environments.

---
Haya Mahmoud/ Airway Avengers
SolveThe17 Hackathon - 15/5/2025
