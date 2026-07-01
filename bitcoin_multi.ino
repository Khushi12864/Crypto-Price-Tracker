#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi Configuration
const char* ssid = "YOUR WIFI";
const char* password = "YOUR PWD";

// TWEAK 1: Modified URL to request bitcoin, ethereum, and dogecoin together
const String multiPriceURL = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum,dogecoin&vs_currencies=usd&include_24hr_change=true";

HTTPClient http;
WiFiClientSecure client; 

// Global variables to hold our parsed data
String coinNames[3] = {"BITCOIN", "ETHEREUM", "DOGECOIN"};
String coinPrices[3] = {"0", "0", "0"};
double coinChanges[3] = {0.0, 0.0, 0.0};

int currentCoinIndex = 0; // Tracks which coin to show on the display (0=BTC, 1=ETH, 2=DOGE)

void printCenter(const String buf, int x, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); 
  display.setCursor((x - w / 2) + (128 / 2), y);
  display.print(buf);
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); 
  }

  display.clearDisplay();
  display.setTextSize(1);           
  display.setTextColor(SSD1306_WHITE);      
  display.setCursor(0,0); 
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nCONNECTED to SSID: ");
  Serial.println(ssid);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connected!");
  display.display();
  delay(2000);

  client.setInsecure(); 
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Fetching multi-crypto data...");

    http.begin(client, multiPriceURL); 
    http.setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    
    int httpCode = http.GET();
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    
    if (httpCode == 200) {
      String payload = http.getString();
      http.end(); 

      // Increased capacity to 512 to easily fit data for all three tokens
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("Parsing failed: "));
        Serial.println(error.f_str());
        delay(5000);
        return;
      }

      // TWEAK 2: Extract data for all 3 coins and save them safely into our arrays
      // For Dogecoin, we use 4 decimal places since its value is under $1
      coinPrices[0] = String(doc["bitcoin"]["usd"].as<double>(), 0);
      coinChanges[0] = doc["bitcoin"]["usd_24h_change"].as<double>();

      coinPrices[1] = String(doc["ethereum"]["usd"].as<double>(), 0);
      coinChanges[1] = doc["ethereum"]["usd_24h_change"].as<double>();

      coinPrices[2] = String(doc["dogecoin"]["usd"].as<double>(), 4); 
      coinChanges[2] = doc["dogecoin"]["usd_24h_change"].as<double>();

      // TWEAK 3: Display the data for the active coin index
      display.clearDisplay();
      
      // Print Asset Label Heading
      display.setTextSize(1);
      printCenter("--- " + coinNames[currentCoinIndex] + " ---", 0, 4);

      // Print Asset Live Value
      display.setTextSize(2); // Larger text for price visibility
      printCenter("$" + coinPrices[currentCoinIndex], 0, 24);

      // Print Percent Change Layout
      display.setTextSize(1);
      String changeStr = "24h Chg: ";
      if (coinChanges[currentCoinIndex] >= 0) {
        changeStr += "+";
      }
      changeStr += String(coinChanges[currentCoinIndex], 2) + "%";
      printCenter(changeStr, 0, 52);
      
      display.display();

      // TWEAK 4: Cycle index sequentially to point to the next coin on the next loop execution
      currentCoinIndex++;
      if (currentCoinIndex >= 3) {
        currentCoinIndex = 0; // Wrap back around to Bitcoin
      }

    } else {
      Serial.print("Error: ");
      Serial.println(http.errorToString(httpCode).c_str()); 
      http.end();
    }
  }
  
  // The display updates and rotates to the next coin every 10 seconds
  delay(10000); 
}