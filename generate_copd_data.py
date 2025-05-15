import csv
import random

# --- Configuration ---
NUM_SAMPLES_PER_SCENARIO = 100  # Generate this many samples for each major scenario block
OUTPUT_FILE = 'simulated_copd_data.csv'

# --- Helper function to generate a data point within ranges ---
def generate_data_point(bpm_range, spo2_range, activity_val, aqi_range, risk_level):
    bpm = random.randint(bpm_range[0], bpm_range[1])
    spo2 = random.randint(spo2_range[0], spo2_range[1])
    # Ensure SpO2 doesn't go above 100
    spo2 = min(spo2, 100)
    aqi = random.randint(aqi_range[0], aqi_range[1])
    return [bpm, spo2, activity_val, aqi, risk_level]

# --- Main data generation list ---
data = []

# --- Scenario 1: Low Risk (Risk = 0) ---
for _ in range(NUM_SAMPLES_PER_SCENARIO):
    # Healthy vitals, resting, good AQI
    data.append(generate_data_point(bpm_range=(60, 90), spo2_range=(95, 100), activity_val=0, aqi_range=(10, 70), risk_level=0))
    # Healthy vitals, active, good AQI
    data.append(generate_data_point(bpm_range=(80, 120), spo2_range=(94, 100), activity_val=1, aqi_range=(10, 80), risk_level=0))
    # Perfect vitals, resting, slightly higher AQI
    data.append(generate_data_point(bpm_range=(65, 85), spo2_range=(97, 100), activity_val=0, aqi_range=(71, 100), risk_level=0))


# --- Scenario 2: Medium Risk (Risk = 1) ---
for _ in range(NUM_SAMPLES_PER_SCENARIO):
    # Good vitals, resting, poor AQI
    data.append(generate_data_point(bpm_range=(60, 90), spo2_range=(95, 100), activity_val=0, aqi_range=(101, 150), risk_level=1))
    # Slightly off SpO2, resting, good AQI
    data.append(generate_data_point(bpm_range=(60, 95), spo2_range=(92, 94), activity_val=0, aqi_range=(20, 100), risk_level=1))
    # Slightly off BPM, resting, good AQI
    data.append(generate_data_point(bpm_range=(91, 98), spo2_range=(95, 100), activity_val=0, aqi_range=(20, 100), risk_level=1))
    # Vitals slightly off during activity, good AQI
    data.append(generate_data_point(bpm_range=(120, 140), spo2_range=(90, 93), activity_val=1, aqi_range=(20, 100), risk_level=1))
    # Good vitals, resting, very high AQI (environmental only concern)
    data.append(generate_data_point(bpm_range=(60,90), spo2_range=(96,100), activity_val=0, aqi_range=(151,200), risk_level=1))


# --- Scenario 3: High Risk (Risk = 2) ---
for _ in range(NUM_SAMPLES_PER_SCENARIO):
    # Moderately low SpO2, resting, any AQI
    data.append(generate_data_point(bpm_range=(70, 100), spo2_range=(89, 91), activity_val=0, aqi_range=(30, 180), risk_level=2))
    # High BPM, resting, any AQI
    data.append(generate_data_point(bpm_range=(99, 110), spo2_range=(92, 100), activity_val=0, aqi_range=(30, 180), risk_level=2))
    # Combo: slightly off SpO2 & BPM, resting, poor AQI
    data.append(generate_data_point(bpm_range=(90, 100), spo2_range=(90, 93), activity_val=0, aqi_range=(101, 180), risk_level=2))
    # Concerning vitals during activity
    data.append(generate_data_point(bpm_range=(141, 160), spo2_range=(88, 90), activity_val=1, aqi_range=(30, 150), risk_level=2))


# --- Scenario 4: Critical Risk (Risk = 3) ---
for _ in range(NUM_SAMPLES_PER_SCENARIO):
    # Very low SpO2, resting, any AQI
    data.append(generate_data_point(bpm_range=(70, 120), spo2_range=(75, 88), activity_val=0, aqi_range=(30, 200), risk_level=3))
    # Very high BPM, resting, lowish SpO2, any AQI
    data.append(generate_data_point(bpm_range=(111, 130), spo2_range=(85, 92), activity_val=0, aqi_range=(30, 200), risk_level=3))
    # Combo: poor SpO2 & BPM, resting, very poor AQI
    data.append(generate_data_point(bpm_range=(101, 120), spo2_range=(80, 89), activity_val=0, aqi_range=(151, 250), risk_level=3))
    # Critical during activity (less likely to be sustained but for dataset)
    data.append(generate_data_point(bpm_range=(150, 180), spo2_range=(70, 87), activity_val=1, aqi_range=(30, 200), risk_level=3))


# --- Add some edge cases or less common scenarios ---
# Healthy but very active, slightly higher BPM, SpO2 fine
data.append(generate_data_point(bpm_range=(130, 150), spo2_range=(95, 100), activity_val=1, aqi_range=(20, 80), risk_level=0))
# Slightly high BPM resting, but excellent SpO2 and AQI
data.append(generate_data_point(bpm_range=(90, 95), spo2_range=(98, 100), activity_val=0, aqi_range=(10, 50), risk_level=0)) # Could be anxiety, still low risk
# Poor AQI but person is active and vitals are holding up okay (might be medium risk due to strain)
data.append(generate_data_point(bpm_range=(100, 130), spo2_range=(93, 97), activity_val=1, aqi_range=(151, 200), risk_level=1))


# --- Shuffle the data to make it less ordered for training ---
random.shuffle(data)

# --- Write to CSV ---
header = ['BPM', 'SpO2', 'Activity', 'AQI', 'RiskLevel']
with open(OUTPUT_FILE, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    writer.writerows(data)

print(f"Generated {len(data)} samples in '{OUTPUT_FILE}'")