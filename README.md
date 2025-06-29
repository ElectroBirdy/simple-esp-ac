# ESP32 DIY Air Conditioner with NTC Sensors

An experimental project using an ESP32 microcontroller to control a repurposed heat pump from a clothes dryer for room cooling. The system monitors temperatures using 10k NTC sensors, provides a web interface for control and monitoring, and includes safety and automation logic.

## Features

- 📡 **Wi-Fi Access Point**: The ESP32 creates its own Wi-Fi network with a built-in web interface.
- 🌡️ **Temperature Monitoring**: 4 analog NTC sensors (10k) monitor:
  - Room temperature
  - Condenser coil temperature
  - Evaporator coil temperature
  - Compressor temperature
- 🎯 **Adjustable Target Temperature**: Set your desired room temperature via the web interface.
- 🔁 **Automatic Cooling Logic**:
  - Compressor and fans run until the room reaches the target temperature.
  - Compressor stops if the condenser temperature drops below 10 °C to prevent icing.
  - The evaporator fan keeps running for 10 minutes after cooling stops to dissipate remaining heat.
- 🔥 **Safety Features**:
  - Compressor cooling fan turns on if it gets too hot.
  - Compressor shuts off at critical temperature and resumes only after cooling down.
- 🖥️ **Manual Relay Control**: Toggle each relay manually from the web interface for testing or override.

## Hardware Requirements

- ESP32 (WROOM module)
- 4x 10k NTC thermistors
- 4x ADC-capable GPIO pins
- 4x relay modules
- Repurposed heat pump (from clothes dryer)
- 5V or 3.3V power supply for sensors and ESP32

## Pin Configuration

| Function                   | ESP32 Pin | Description                 |
|----------------------------|-----------|-----------------------------|
| Compressor temperature     | A0 (GPIO36)| NTC thermistor              |
| Condenser temperature      | A3 (GPIO39)| NTC thermistor              |
| Evaporator temperature     | A6 (GPIO34)| NTC thermistor              |
| Room temperature           | A7 (GPIO35)| NTC thermistor              |
| Compressor relay           | GPIO16     | Turns compressor on/off     |
| Condenser fan relay        | GPIO17     | Controls condenser fan      |
| Evaporator fan relay       | GPIO18     | Controls evaporator fan     |
| Compressor cooler relay    | GPIO19     | Cools down hot compressor   |

## How to Use

1. Upload the code to your ESP32 using Arduino IDE or PlatformIO.
2. The ESP32 creates a Wi-Fi network called `ESP32-Climate` (password: `12345678`).
3. Connect to the network and open a browser at `http://192.168.4.1`.
4. View real-time temperatures, set your desired temperature, and control relays.

## NTC Sensor Configuration

You can adjust the characteristics of your thermistors in the code:

```cpp
#define BETA 3435           // Thermistor B-value
#define R_FIXED 10000       // Fixed resistor in voltage divider (ohms)
