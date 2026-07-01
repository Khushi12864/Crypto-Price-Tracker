# ESP32 Crypto Price Tracker ₿

A mini IoT project that fetches and displays live cryptocurrency 
prices on an OLED screen using an ESP32.

## What it does
- Fetches real-time prices for Bitcoin, Ethereum, and Dogecoin
  via the CoinGecko API
- Displays price and 24h % change on a 128x64 OLED screen
- Cycles through all three coins every 10 seconds

## Components Used
- ESP32 Dev Module
- 0.96" OLED Display (SSD1306, I2C)
- Jumper Wires

## Libraries Used
- Adafruit_SSD1306.h
- Wire.h
- WiFi.h
- HTTPClient.h
- ArduinoJson.h

## Setup
1. Install the required libraries in Arduino IDE
2. Replace YOUR_WIFI_SSID and YOUR_WIFI_PASSWORD with your credentials
3. Connect OLED to ESP32 via I2C (SDA, SCL)
4. Upload and monitor via Serial Monitor at 115200 baud

## API Used
CoinGecko Free API — no key required

## Learnings
Built this with zero prior knowledge of crypto. 
Focused on API calls, JSON parsing, and OLED display rendering 
on embedded hardware.
