#include <Arduino.h>
#include <deneyap.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Veritabanına düzgün bağlanmak için gerekli olan ilaveler
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Ağ bilgilerinizi girin
#define WIFI_SSID "Note8"
#define WIFI_PASSWORD "31313131"

// Firebase proje API Key'i girin
#define API_KEY "AIzaSyDSKK5NeFLKnsiwud0b388RWVSLRG2kxFA"

// Veritabanı URL'sini girin
#define DATABASE_URL "https://fir-app-d53b5-default-rtdb.firebaseio.com/"

// Su sensörünün bağlı olduğu pin
int waterSensorPin = A0;
// Buzzer'ın bağlı olduğu pin
int buzzerPin = D12;

// Hareket sensörünün bağlı olduğu pin
int motionSensorPin = D1;

// Firebase nesneleri
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
unsigned long sendDataPrevMillis = 0;
unsigned long getDataPrevMillis = 0;

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupFirebase();

  pinMode(motionSensorPin, INPUT);
  pinMode(waterSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int motionValue = digitalRead(motionSensorPin);
  int waterLevel = analogRead(waterSensorPin);

  buzzOnMotion(motionValue);

  sendDataToFirebase(motionValue, waterLevel);
  getDataFromFirebase();
}

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Bağlanıyor");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Bağlandı. IP Adresi: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase bağlantısı başarılı");
    signupOK = true;
  } else {
    Serial.println("Firebase bağlantı hatası:");
    Serial.println(config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void buzzOnMotion(int motionValue) {
  if (motionValue == HIGH) {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
  }
}

void sendDataToFirebase(int motionValue, int waterLevel) {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    if (!Firebase.RTDB.setInt(&fbdo, "test/motion_sensor", motionValue)) {
      Serial.println("Hata: Hareket sensörü verisi gönderilemedi");
      Serial.println("Hata sebebi: " + fbdo.errorReason());
    }

    if (!Firebase.RTDB.setInt(&fbdo, "test/water_level", waterLevel)) {
      Serial.println("Hata: Su seviyesi verisi gönderilemedi");
      Serial.println("Hata sebebi: " + fbdo.errorReason());
    }
  }
}

void getDataFromFirebase() {
  if (Firebase.ready() && signupOK && (millis() - getDataPrevMillis > 1000 || getDataPrevMillis == 0)) {
    getDataPrevMillis = millis();

    if (Firebase.RTDB.getInt(&fbdo, "/test/motion_sensor")) {
      if (fbdo.dataType() == "int") {
        int motionValue = fbdo.intData();
        Serial.println("Hareket Sensörü Değeri: " + String(motionValue));
      }
    } else {
      Serial.println("Hata: Hareket sensörü verisi alınamadı");
      Serial.println("Hata sebebi: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getInt(&fbdo, "/test/water_level")) {
      if (fbdo.dataType() == "int") {
        int waterLevel = fbdo.intData();
        Serial.println("Su Seviyesi: " + String(waterLevel));
      }
    } else {
      Serial.println("Hata: Su seviyesi verisi alınamadı");
      Serial.println("Hata sebebi: " + fbdo.errorReason());
    }
  }
}
