# Pathfinder

Pathfinder is an advanced navigation and sensor system designed for off-road jeeps, providing crucial data and location services in remote environments.

## Project Overview

Pathfinder integrates various sensors with an ESP32 microcontroller to collect and transmit vital information to a mobile application via Bluetooth. This system enhances safety and navigation capabilities for off-road adventures.

### Key Features

- GPS coordinate saving and retrieval
- Real-time sensor data transmission
- Last known location recovery
- Bluetooth connectivity with mobile app

### Sensors

The system incorporates multiple sensors to provide comprehensive vehicle data:

- Tilt sensor
- Magnetometer (compass)
- Temperature sensor (for coolant)
- Voltage sensors (for battery and solar panel)

## System Architecture

### Hardware

- ESP32 microcontroller
- SD card for data storage
- Various sensors (as listed above)
- Bluetooth module (integrated with ESP32)

### Software

- ESP32 firmware (C++ with Arduino framework)
- FreeRTOS for task management

## Functionality

1. **Pre-trip Preparation**:
   - User sends GPS coordinates to the ESP32 via the mobile app
   - Coordinates are saved on the SD card

2. **During Trip**:
   - Continuous sensor data transmission to the mobile app
   - Real-time display of vehicle status and environmental conditions

3. **Emergency Situation**:
   - User can request saved GPS coordinates
   - App calculates direction to the last known location using compass data

## Development Status

The project is currently in the development phase. The basic structure for Bluetooth communication, data saving, and sensor reading is implemented. Placeholder code (using an ultrasonic sensor) demonstrates the data flow, which will be replaced with actual sensor implementations.

## Next Steps

- Integration of actual sensors (tilt, compass, temperature, voltage)
- Development of the mobile application
- Comprehensive testing in various off-road conditions

## Contributing

We welcome contributions to the Pathfinder project. Please read our contributing guidelines before submitting pull requests.
