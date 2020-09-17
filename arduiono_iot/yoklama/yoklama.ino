#include <Arduino.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h> 

#define RST_PIN 9
#define SS_PIN 4
#define ledPin 7

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

char ssid[]="ssid";//kablosuz ssid 
char pass[]="parola";//ssid parolası
String srv="sunucu_adresi";//sunucu adresi
String kartNo="";
String rfid="";

ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
   SPI.begin();
  mfrc522.PCD_Init();
  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid,pass);
 

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    

    WiFiClient client;

    HTTPClient http;

    //rfid kısmı
       if ( ! mfrc522.PICC_IsNewCardPresent())
      {
        return;
      }
      if ( ! mfrc522.PICC_ReadCardSerial())
      {
        return;
      }
      //kartın UID'sini oku, rfid isimli string'e kaydet
      rfid = "";
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        rfid += mfrc522.uid.uidByte[i] < 0x10 ? "0" : "";
        rfid += String(mfrc522.uid.uidByte[i], HEX);
      }
      //string'in boyutunu ayarla ve tamamını büyük harfe çevir
      rfid.trim();
      rfid.toUpperCase();
   
    
  
    //Verinin ağ dan gönderilmesi
    kartNo=rfid;
    //rfid kısmı

    Serial.print("[HTTP] baslatildi...\n");
    if (http.begin(client, "http://"+srv+":5000/kartnumarasi/"+kartNo)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // http bağlantısı başlatıldı
      int httpCode = http.GET();

      // httpCode bilgisi alınıyor
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // Sunucuya ulaşılamadı
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... hata: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP] bağlantı kurulamadi\n");
    }
  }

  delay(10000);
}
