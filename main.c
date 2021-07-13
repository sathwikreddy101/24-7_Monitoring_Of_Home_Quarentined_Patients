include "FS.h" 
#include <Wire.h>
#include "MAX30100_PulseOximeter.h" 
#define CAYENNE_PRINT Serial 
#include <CayenneMQTTESP8266.h> 
#define REPORTING_PERIOD_MS 10000
PulseOximeter pox; 
float BPM, SpO2;
uint32_t tsLastReport = 0; 
String hrData = "";
unsigned long timems =0; 
char ssid[] = "StealthSpidy";
char wifiPassword[] = "11121999";
char username[] = "d910a760-57b0-11eb-a2e4-b32ea624e442";
char password[] = "82912c16e5dbbc0e6eb82a4cf1e375e0813e97bd"; 
char clientID[] = "0f0dc280-57b1-11eb-8779-7d56e82df461";
void onBeatDetected()
{
Serial.println("Beat!");
}
void setup() { 
Serial.begin(115200); 
pinMode(2, OUTPUT);
Cayenne.begin(username, password, clientID, ssid, wifiPassword); 
Serial.print("Initializing Pulse Oximeter..");
pinMode(16, OUTPUT);
if (!pox.begin())
{
Serial.println("FAILED"); 
for(;;);
}
else
{
Serial.println("SUCCESS");
digitalWrite(2, HIGH); //Turn off in-built LED
}
pox.setOnBeatDetectedCallback(onBeatDetected); 
pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
if(!SPIFFS.begin()){
Serial.println("An Error has occurred while mounting SPIFFS"); 
return;
}
}
void loop() {
pox.update();
BPM = pox.getHeartRate(); 
SpO2 = pox.getSpO2();
if (BPM < 40 || SpO2 == 0) {
Serial.println(F("low value from sensor!")); 
return;
}
if (millis() - tsLastReport > REPORTING_PERIOD_MS)
{
digitalWrite(2, LOW):
Serial.print("Heart rate: "); Serial.print(BPM); 
timems = millis();
hrData = String(timems) + String(",") + String(BPM) + String(",") + 
String(SpO2); //convert variable from integer to string
Serial.print(" bpm , SpO2:"); Serial.print(SpO2); Serial.println(" %"); 
Serial.println (hrData);
File file = SPIFFS.open("/HR_test.txt", "a"); 
Cayenne.virtualWrite(0, BPM, "counter","null" ); 
Cayenne.virtualWrite(1, SpO2, "O2", "p");
if(!file){
Serial.println("Failed to open file for writing"); 
return;
}
file.println (hrData); 
file.close(); 
digitalWrite(2, HIGH); 
tsLastReport = millis();
}
}
