#include <WiFi.h>
#include <WebServer.h>

// WiFi Access Point
const char* ssid = "ESP32-Klima";
const char* password = "geslo1234";

WebServer server(80);

// Analogni pini za NTC senzorje
const int pinRoom = 34;
const int pinCond = 35;
const int pinEvap = 32;
const int pinComp = 33;

// Digitalni pini za releje
const int relayComp = 25;
const int relayFanCond = 26;
const int relayFanEvap = 27;
const int relayFanComp = 14;

// Spremenljivke stanja in logike
float targetTemp = 23.0;
const float hysteresis = 0.5;

const float maxCompTemp = 85.0;
const float fanCompOnTemp = 60.0;
const float minCondTemp = 10.0;

unsigned long evapFanOffDelay = 10 * 60 * 1000;
unsigned long evapFanOffTime = 0;
bool coolingActive = false;

// Manual override za vsak rele (0 = avtomatika, 1 = ročno ON, -1 = ročno OFF)
int manualComp = 0;
int manualFanCond = 0;
int manualFanEvap = 0;
int manualFanComp = 0;

// Parametri za NTC senzor (10k Ohm, B=3435)
const float nominalResistance = 10000.0;  // 10k Ohm
const float nominalTemp = 25.0;           // 25°C
const float bCoefficient = 3435.0;        // B-parameter
const float seriesResistance = 10000.0;   // 10k Ohm delilnik

// Pretvori analogno NTC v temperaturo z upoštevanjem B-parametra
float readNTC(int pin) {
  int adc = analogRead(pin);
  if (adc == 0) return -20.0;  // Prepreči deljenje z nič
  float voltage = adc * 3.3 / 4095.0;
  float resistance = seriesResistance * (3.3 - voltage) / voltage;
  float steinhart = log(resistance / nominalResistance) / bCoefficient + 1.0 / (nominalTemp + 273.15);
  float temp = (1.0 / steinhart) - 273.15;
  return constrain(temp, -20, 100);
}

// Posodobi rele glede na manualOverride (če 0, uporabi autoValue)
void updateRelay(int pin, int manualState, bool autoValue) {
  if (manualState == 1) digitalWrite(pin, HIGH);
  else if (manualState == -1) digitalWrite(pin, LOW);
  else digitalWrite(pin, autoValue ? HIGH : LOW);
}

// HTML stran
void handleRoot() {
  float tRoom = readNTC(pinRoom);
  float tCond = readNTC(pinCond);
  float tEvap = readNTC(pinEvap);
  float tComp = readNTC(pinComp);

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>ESP32 Klima</title></head><body>";
  html += "<h2>Temperature:</h2>";
  html += "Soba: " + String(tRoom, 1) + " °C<br>";
  html += "Kondenzator: " + String(tCond, 1) + " °C<br>";
  html += "Evaporator: " + String(tEvap, 1) + " °C<br>";
  html += "Kompresor: " + String(tComp, 1) + " °C<br><hr>";

  html += "<h2>Stanje relejev:</h2>";
  html += "Kompresor: " + String(digitalRead(relayComp) ? "ON" : "OFF") + " ";
  html += "[<a href='/manual?relay=comp&state=on'>ON</a> | <a href='/manual?relay=comp&state=off'>OFF</a> | <a href='/manual?relay=comp&state=auto'>AUTO</a>]<br>";

  html += "Vent. kondenzator: " + String(digitalRead(relayFanCond) ? "ON" : "OFF") + " ";
  html += "[<a href='/manual?relay=fancond&state=on'>ON</a> | <a href='/manual?relay=fancond&state=off'>OFF</a> | <a href='/manual?relay=fancond&state=auto'>AUTO</a>]<br>";

  html += "Vent. evaporator: " + String(digitalRead(relayFanEvap) ? "ON" : "OFF") + " ";
  html += "[<a href='/manual?relay=fanevap&state=on'>ON</a> | <a href='/manual?relay=fanevap&state=off'>OFF</a> | <a href='/manual?relay=fanevap&state=auto'>AUTO</a>]<br>";

  html += "Vent. kompresor: " + String(digitalRead(relayFanComp) ? "ON" : "OFF") + " ";
  html += "[<a href='/manual?relay=fancomp&state=on'>ON</a> | <a href='/manual?relay=fancomp&state=off'>OFF</a> | <a href='/manual?relay=fancomp&state=auto'>AUTO</a>]<br><hr>";

  html += "<h2>Nastavi ciljno temperaturo</h2>";
  html += "<form action='/set' method='GET'>";
  html += "<input type='number' step='0.1' name='temp' value='" + String(targetTemp, 1) + "'> °C";
  html += "<input type='submit' value='Potrdi'>";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("temp")) {
    targetTemp = server.arg("temp").toFloat();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// Ročno upravljanje relejev
void handleManual() {
  String relay = server.arg("relay");
  String state = server.arg("state");

  int val = 0;
  if (state == "on") val = 1;
  else if (state == "off") val = -1;
  else val = 0;

  if (relay == "comp") manualComp = val;
  if (relay == "fancond") manualFanCond = val;
  if (relay == "fanevap") manualFanEvap = val;
  if (relay == "fancomp") manualFanComp = val;

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(relayComp, OUTPUT);
  pinMode(relayFanCond, OUTPUT);
  pinMode(relayFanEvap, OUTPUT);
  pinMode(relayFanComp, OUTPUT);
  digitalWrite(relayComp, LOW);
  digitalWrite(relayFanCond, LOW);
  digitalWrite(relayFanEvap, LOW);
  digitalWrite(relayFanComp, LOW);

  WiFi.softAP(ssid, password);
  Serial.print("WiFi AP zagnan. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/manual", handleManual);
  server.begin();
}

void loop() {
  server.handleClient();

  float tRoom = readNTC(pinRoom);
  float tCond = readNTC(pinCond);
  float tEvap = readNTC(pinEvap);
  float tComp = readNTC(pinComp);

  unsigned long now = millis();

  bool autoComp = false;
  bool autoFanCond = false;
  bool autoFanEvap = false;
  bool autoFanComp = false;

  // Varnost kompresorja
  if (tComp > maxCompTemp) {
    coolingActive = false;
    autoComp = false;
    autoFanComp = true;
  } else {
    // Kompresor ventilator pri srednji temperaturi
    autoFanComp = (tComp > fanCompOnTemp);

    // Avtomatsko vklop/izklop hlajenja
    if (coolingActive) {
      if (tRoom <= targetTemp - hysteresis || tCond < minCondTemp) {
        coolingActive = false;
        evapFanOffTime = now + evapFanOffDelay;
      }
    } else {
      if (tRoom >= targetTemp + hysteresis && tCond >= minCondTemp && tComp < maxCompTemp) {
        coolingActive = true;
      }
    }

    autoComp = coolingActive;
    autoFanCond = coolingActive;
  }

  // Evaporator ventilator
  if (coolingActive || now < evapFanOffTime) {
    autoFanEvap = true;
  }

  updateRelay(relayComp, manualComp, autoComp);
  updateRelay(relayFanCond, manualFanCond, autoFanCond);
  updateRelay(relayFanEvap, manualFanEvap, autoFanEvap);
  updateRelay(relayFanComp, manualFanComp, autoFanComp);

  delay(500);
}