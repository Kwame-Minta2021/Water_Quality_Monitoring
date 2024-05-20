# Water Quality Monitoring

## Overview
This project involves monitoring various parameters of water quality using an ESP32 microcontroller. The parameters measured include pH, turbidity, Total Dissolved Solids (TDS), and flow rate. The collected data is sent to the Blynk cloud platform for remote monitoring and visualization.

## Features
- **pH Measurement**: Monitors the acidity or alkalinity of the water.
- **Turbidity Measurement**: Measures the clarity of the water.
- **TDS Measurement**: Determines the amount of dissolved solids in the water.
- **Flow Rate Measurement**: Monitors the flow rate of the water.
- **Remote Monitoring**: Data is sent to Blynk cloud for real-time monitoring and visualization.

## Components Used
- **ESP32**: The main microcontroller used for data acquisition and communication.
- **pH Sensor**: Measures the pH level of the water.
- **Turbidity Sensor**: Measures the turbidity or cloudiness of the water.
- **TDS Sensor**: Measures the Total Dissolved Solids in the water.
- **Flow Sensor**: Measures the flow rate of the water.

## Hardware Setup
1. Connect the pH sensor to an analog input pin on the ESP32.
2. Connect the turbidity sensor to another analog input pin on the ESP32.
3. Connect the TDS sensor to yet another analog input pin on the ESP32.
4. Connect the flow sensor to a digital input pin on the ESP32.
5. Power the sensors as per their operating voltage requirements, typically 3.3V or 5V.
6. Ensure all sensor grounds are connected to the ESP32 ground.

## Software Setup
### Install the Arduino IDE and set up the ESP32 board:
1. Add the ESP32 board to the Arduino IDE by following the instructions from the [ESP32 Arduino core repository](https://github.com/espressif/arduino-esp32).

### Install Blynk Library:
1. In the Arduino IDE, go to `Sketch -> Include Library -> Manage Libraries`.
2. Search for "Blynk" and install the Blynk library.

### Configure Blynk:
1. Create a new project in the Blynk app.
2. Note the Auth Token sent to your email.
3. Add the appropriate widgets to the Blynk project for pH, turbidity, TDS, and flow rate readings.

