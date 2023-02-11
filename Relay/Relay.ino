/*
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
       #include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define WIFI_SSID         "hidden_network"    
#define WIFI_PASS         "itsnotzigbee"
#define APP_KEY           "b906a9e4-03cb-4c88-8503-4a41eda5b331"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "ad7a246d-f213-4eea-b9aa-648a78d4a9d0-429533fe-c970-4abb-80c7-a8028b086377"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_ID         "63e6131e5ec7d92a4707073d"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define SWITCH_ID1        "63e5cc451bb4e19c1197cfd6"  
#define SWITCH_ID2        "63e608721bb4e19c1197eb09"
#define BAUD_RATE         9600                // Change baudrate to your need


#ifdef ESP8266
#define RELAY_PIN         D5                  // Pin where the relay is connected (D5 = GPIO 14 on ESP8266)
#define RELAY_PIN1        D6                  // Pin where the relay is connected (D5 = GPIO 14 on ESP8266)
#define RELAY_PIN2        D7                  // Pin where the relay is connected (D5 = GPIO 14 on ESP8266)
#endif

#ifdef ESP32
#define RELAY_PIN         16                  // Pin where the relay is connected (GPIO 16 on ESP32)
#endif

bool onPowerState1(const String &deviceId, bool &state) {
  digitalWrite(RELAY_PIN, state);             // set pin state
  return true;                                // request handled properly
}

bool onPowerState2(const String &deviceId, bool &state) {
  Serial.printf("Device 2 turned %s\r\n", state?"on":"off");
  digitalWrite(RELAY_PIN1, state);             // set pin state        
  return true; // request handled properly
}

bool onPowerState3(const String &deviceId, bool &state) {
  Serial.printf("Device 3 turned %s\r\n", state?"on":"off");
  digitalWrite(RELAY_PIN2, state);             // set pin state        
  return true; // request handled properly
}


void setup() {
  pinMode(RELAY_PIN, OUTPUT);                 // set relay-pin to output mode
   pinMode(RELAY_PIN1, OUTPUT);                 // set relay-pin to output mode
      pinMode(RELAY_PIN2, OUTPUT);   
  WiFi.begin(WIFI_SSID, WIFI_PASS);           // start wifi
Serial.begin(9600);
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];   // create new switch device
  mySwitch.onPowerState(onPowerState1);                // apply onPowerState callback

    SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID1];   // create new switch device
  mySwitch1.onPowerState(onPowerState2);                // apply onPowerState callback

      SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID2];   // create new switch device
  mySwitch2.onPowerState(onPowerState3);                // apply onPowerState callback


  SinricPro.begin(APP_KEY, APP_SECRET);               // start SinricPro
   while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname("home1");

  // No authentication by default
ArduinoOTA.setPassword("homefirst");
ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

}

void loop() {
  SinricPro.handle();                         // handle SinricPro commands
  ArduinoOTA.handle(); 
}
