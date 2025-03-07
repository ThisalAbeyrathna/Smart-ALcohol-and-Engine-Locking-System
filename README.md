# Automatic Alcohol Detection and Engine Locking System 🚗🔒

This project is an **ESP32-based Alcohol Detection System** that monitors the driver's alcohol level and automatically **locks the engine** if intoxication is detected. The system also **sends GPS coordinates** to a server for further action.

## Features ✨
- **Alcohol detection** using an MQ-3 (or similar) alcohol sensor.
- **Real-time GPS tracking** to send location data when intoxication is detected.
- **Alert mechanisms**:
  - LCD warning messages.
  - Buzzer and LED activation.
  - Engine (motor) locking.
- **WiFi-enabled** connectivity for sending data to a remote server.
- **Failsafe mechanism** that stops the system after detecting alcohol.

## Hardware Components 🛠️
- **ESP32** (Main controller)
- **Alcohol Sensor** (Connected to analog pin)
- **LCD Display** (I2C-based 16x2 LCD)
- **Buzzer** (Alert mechanism)
- **LED Indicator** (Warning signal)
- **GPS Module** (Location tracking)
- **Motor (Engine Representation)** (Locks when intoxicated)
- **WiFi Connectivity** (For sending GPS data)

## Working Principle ⚙️
1. **System Initialization**:
   - Connects to WiFi.
   - Displays "Scanning..." on the LCD.
   
2. **Alcohol Level Detection**:
   - Reads sensor data for 30 seconds.
   - If **alcohol level > threshold**, triggers alerts and locks the engine.

3. **Alerts** (If intoxicated):
   - Displays "You are drunk!"
   - Turns on **LED and buzzer**.
   - Stops the **motor** (Engine locking).

4. **GPS Location Sending**:
   - Retrieves latitude and longitude.
   - Sends data to a remote server via an HTTP request.

5. **System Shutdown**:
   - If safe: Displays "Safe to drive" and stops execution.
   - If drunk: Displays messages and enters an infinite loop to prevent driving.

## Installation & Usage 🚀
1. **Clone this repository**:
   ```sh
   git clone https://github.com/your-username/AutomaticAlcoholDetection.git
