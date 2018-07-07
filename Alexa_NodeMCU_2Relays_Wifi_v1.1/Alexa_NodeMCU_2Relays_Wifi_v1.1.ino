/* Home Automation With Alexa and ESP8266 / NodeMCU_CP2102_V2
Created by Exl Lab, 2018/07/04. Version 1.1 (wifiManager)
Note: 
- Replace the original WiFiManager.h and WiFiManager.cpp files (under <Arduino_IDE_dir>/libraries/WiFiManager/) 
  with 2 files: WiFiManager.h and WiFiManager.cpp in the folder: "WiFiManagerLib for fauxmoESP".
- Edit lines from  17 to 22 for your config.
*/

#define WIFI_MANAGER_USE_ASYNC_WEB_SERVER
#include <WiFiManager.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
fauxmoESP fauxmo;

// Device Names:
const char* Device_Name_1 = "Light one";
const char* Device_Name_2 = "Light two";

// select pin on ESP/NodeMCU to control Relay
const byte RelayPin_1 = 14; //D5 - GPIO14
const byte RelayPin_2 = 12; //D6 - GPIO12

char ESP_SSID_Name[6];

bool flag_SaveConfig = false;
void check_saveConfig () {
  Serial.println("Save config");
  flag_SaveConfig = true;
}

//Main setup
void setup() {
   Serial.begin(115200);
   //setup and wifi connection
  delay(10);

   sprintf(ESP_SSID_Name, "%06X", ESP.getChipId());
   WiFiManager wifiManager;
   wifiManager.setSaveConfigCallback(check_saveConfig);
   if (!wifiManager.autoConnect(ESP_SSID_Name)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
   }  
   Serial.println("Wifi connected!!!");
  
   // Device Names for Simulated Wemo switches
   AddDevice2Alexa(RelayPin_1, Device_Name_1);
   AddDevice2Alexa(RelayPin_2, Device_Name_2);
   fauxmo.onMessage(callback); 
}

// loop
void loop() {
  fauxmo.handle();
}

// Device Callback
void callback(uint8_t device_id, const char *device_Alexa, bool stateRelay) {
  // Switching action on detection of device name
  callbackDevice(Device_Name_1, device_Alexa, RelayPin_1, stateRelay);  // RelayPin_1
  callbackDevice(Device_Name_2, device_Alexa, RelayPin_2, stateRelay);  // RelayPin_2
}

void AddDevice2Alexa(byte _Relay_Pin, const char *_Device_Name) {
  // Set relay pins to outputs
  pinMode(_Relay_Pin, OUTPUT);

  // Device Names for Simulated Wemo switches
  fauxmo.addDevice(_Device_Name);
  delay(500);
}; 

void callbackDevice(const char *_Device_Name, const char *_device_Alexa, byte _Relay_Pin, bool _stateRelay) {
  if (!String(_Device_Name).equals(_device_Alexa)) return;

  if (_stateRelay) {
    digitalWrite(_Relay_Pin, LOW); 
    Serial.println(String(_Device_Name) + " is ON");
    return;
  }

  digitalWrite(_Relay_Pin, HIGH);
  Serial.println(String(_Device_Name) + " is OFF");
};