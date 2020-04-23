#include <user_interface.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#define LED_PIN 2
#define LED_LEVEL LOW

static const char WIFI_SSID[] PROGMEM = "ESP14";
static const char WIFI_PSWD[] PROGMEM = "1029384756";

static bool prepareWiFi() {
  wifi_country_t wc;

  wc.cc[0] = 'J';
  wc.cc[1] = 'P';
  wc.cc[2] = '\0';
  wc.schan = 1;
  wc.nchan = 14;
  wc.policy = WIFI_COUNTRY_POLICY_MANUAL;
  return wifi_set_country(&wc);
}

static bool connectWiFi(uint32_t timeout) {
  uint32_t start = millis();

  WiFi.mode(WIFI_STA);
  Serial.print(F("Connecting to \""));
  Serial.print(FPSTR(WIFI_SSID));
  Serial.print('"');
  WiFi.begin(FPSTR(WIFI_SSID), FPSTR(WIFI_PSWD), 14);
  while (! WiFi.isConnected()) {
    if (millis() - start >= timeout)
      break;
    Serial.write('.');
    digitalWrite(LED_PIN, LED_LEVEL);
    delay(25);
    digitalWrite(LED_PIN, ! LED_LEVEL);
    delay(500 - 25);
  }
  if (WiFi.isConnected()) {
    Serial.print(F(" OK (IP "));
    Serial.print(WiFi.localIP());
    Serial.print(F(", channel "));
    Serial.print(WiFi.channel());
    Serial.println(')');
    return true;
  } else {
    WiFi.disconnect();
    Serial.println(F(" FAIL!"));
    return false;
  }
}

void setup() {
  const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ! LED_LEVEL);

  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.println();

  if (! prepareWiFi()) {
    Serial.println(F("wifi_set_country() fail!"));
    Serial.flush();
    ESP.deepSleep(0);
  }

  WiFi.persistent(false);

  if (! connectWiFi(WIFI_TIMEOUT)) {
    WiFi.mode(WIFI_AP);
    Serial.print(F("Creating AP \""));
    Serial.print(FPSTR(WIFI_SSID));
    Serial.print(F("\" with password \""));
    Serial.print(FPSTR(WIFI_PSWD));
    Serial.print(F("\" on channel 14 "));
    if (WiFi.softAP(FPSTR(WIFI_SSID), FPSTR(WIFI_PSWD), 14)) {
      Serial.println(F("OK, waiting for clients..."));
    } else {
      Serial.println(F("FAIL!"));
      Serial.flush();
      ESP.restart();
    }
  }
}

void loop() {}
