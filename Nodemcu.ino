#define BLYNK_TEMPLATE_ID "TMPL6lLmKejUi"
#define BLYNK_TEMPLATE_NAME "Sera"
#define BLYNK_AUTH_TOKEN "VavOykp1ywvchN3JiCYjIPberHxEYODW"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

// WiFi bilgileri
char ssid[] = "Sera";
char pass[] = "12345678";

// Nano UART
SoftwareSerial nanoSerial(D7, D6); // RX=D7, TX=D6

String data = "";

void setup() {
  Serial.begin(115200);
  nanoSerial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Blynk baglandi...");
}

void loop() {
  Blynk.run();

  if (nanoSerial.available()) {
    data = nanoSerial.readStringUntil('\n');

    Serial.println(data);

    parseData(data);
  }
}

// ===== PARSE + BLYNK GÖNDER =====
void parseData(String d) {

  int T1, T2, S1, S2, L1, L2, SU1, SU2;
  int TANK, M1, M2, FAN;
  float SIC, NEM;

  int ok = sscanf(d.c_str(),
    "T1:%d,T2:%d,S1:%d,S2:%d,L1:%d,L2:%d,SIC:%f,NEM:%f,SU1:%d,SU2:%d,TANK:%d,M1:%d,M2:%d,FAN:%d",
    &T1, &T2, &S1, &S2, &L1, &L2, &SIC, &NEM, &SU1, &SU2,
    &TANK, &M1, &M2, &FAN
  );

  if (ok < 14) return;

  Blynk.virtualWrite(V0, T1);
  Blynk.virtualWrite(V1, T2);

  Blynk.virtualWrite(V2, S1);
  Blynk.virtualWrite(V3, S2);

  Blynk.virtualWrite(V4, L1);
  Blynk.virtualWrite(V5, L2);

  Blynk.virtualWrite(V6, SIC);
  Blynk.virtualWrite(V7, NEM);

  Blynk.virtualWrite(V8, SU1);
  Blynk.virtualWrite(V9, SU2);

  Blynk.virtualWrite(V10, TANK);
  Blynk.virtualWrite(V11, M1);
  Blynk.virtualWrite(V12, M2);
  Blynk.virtualWrite(V13, FAN);
}