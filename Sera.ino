#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>

// OLED ekran ayarları
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G ekran = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT11
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Analog girişler
const int analogPin0 = A0;
const int analogPin1 = A1;

// Dijital giriş
const int dijitalGirisPin = 6;

// Çıkışlar
const int cikisPinA0 = 7;
const int cikisPinA1 = 8;

// Encoder pinleri
const int encoderCLK = 2;
const int encoderDT  = 3;
const int encoderSW  = 4;

// Hedef değerler
int hedefDegerA0 = 500;
int hedefDegerA1 = 500;

// Mod
// 0 = ayar yok
// 1 = A0 hedef ayarı
// 2 = A1 hedef ayarı
int ayarModu = 0;

// Encoder değişkenleri
int sonCLK = HIGH;

// Buton değişkenleri
int sonButonOkuma = HIGH;
int butonDurumu = HIGH;
unsigned long sonButonDegisimZamani = 0;
const unsigned long butonDebounce = 80;

// DHT değişkenleri
float nem = 0;
float sicaklik = 0;
unsigned long sonDHTOkuma = 0;

// Ekran yenileme
unsigned long sonEkranYenileme = 0;
const unsigned long ekranYenilemeAraligi = 200;

void setup() {
  pinMode(dijitalGirisPin, INPUT);

  pinMode(cikisPinA0, OUTPUT);
  pinMode(cikisPinA1, OUTPUT);

  digitalWrite(cikisPinA0, LOW);
  digitalWrite(cikisPinA1, LOW);

  pinMode(encoderCLK, INPUT_PULLUP);
  pinMode(encoderDT, INPUT_PULLUP);
  pinMode(encoderSW, INPUT_PULLUP);

  sonCLK = digitalRead(encoderCLK);

  Wire.begin();

  ekran.begin(0x3C, true);
  ekran.clearDisplay();
  ekran.setTextColor(SH110X_WHITE);
  ekran.setTextSize(1);
  ekran.setCursor(0, 0);
  ekran.println("Sistem basladi");
  ekran.display();

  dht.begin();

  delay(1000);
}

void loop() {
  int analogDeger0 = analogRead(analogPin0);
  int analogDeger1 = analogRead(analogPin1);
  int dijitalDeger = digitalRead(dijitalGirisPin);

  encoderYonOku();
  encoderButonOku();

  // A0 kontrol
  if (analogDeger0 < hedefDegerA0) {
    digitalWrite(cikisPinA0, HIGH);
  } else {
    digitalWrite(cikisPinA0, LOW);
  }

  // A1 kontrol
  if (analogDeger1 < hedefDegerA1) {
    digitalWrite(cikisPinA1, HIGH);
  } else {
    digitalWrite(cikisPinA1, LOW);
  }

  // DHT11 2 saniyede bir okunur
  if (millis() - sonDHTOkuma >= 2000) {
    sonDHTOkuma = millis();

    float okunanNem = dht.readHumidity();
    float okunanSicaklik = dht.readTemperature();

    if (!isnan(okunanNem)) {
      nem = okunanNem;
    }

    if (!isnan(okunanSicaklik)) {
      sicaklik = okunanSicaklik;
    }
  }

  // OLED ekran yenileme
  if (millis() - sonEkranYenileme >= ekranYenilemeAraligi) {
    sonEkranYenileme = millis();
    ekranYaz(analogDeger0, analogDeger1, dijitalDeger);
  }
}

void encoderYonOku() {
  int yeniCLK = digitalRead(encoderCLK);

  if (yeniCLK != sonCLK) {
    if (yeniCLK == LOW) {
      if (digitalRead(encoderDT) == HIGH) {
        degerArttir();
      } else {
        degerAzalt();
      }
    }
  }

  sonCLK = yeniCLK;
}

void encoderButonOku() {
  int okuma = digitalRead(encoderSW);

  if (okuma != sonButonOkuma) {
    sonButonDegisimZamani = millis();
  }

  if ((millis() - sonButonDegisimZamani) > butonDebounce) {
    if (okuma != butonDurumu) {
      butonDurumu = okuma;

      if (butonDurumu == LOW) {
        ayarModu++;

        if (ayarModu > 2) {
          ayarModu = 0;
        }
      }
    }
  }

  sonButonOkuma = okuma;
}

void degerArttir() {
  if (ayarModu == 1) {
    hedefDegerA0 += 5;

    if (hedefDegerA0 > 1023) {
      hedefDegerA0 = 1023;
    }
  }

  if (ayarModu == 2) {
    hedefDegerA1 += 5;

    if (hedefDegerA1 > 1023) {
      hedefDegerA1 = 1023;
    }
  }
}

void degerAzalt() {
  if (ayarModu == 1) {
    hedefDegerA0 -= 5;

    if (hedefDegerA0 < 0) {
      hedefDegerA0 = 0;
    }
  }

  if (ayarModu == 2) {
    hedefDegerA1 -= 5;

    if (hedefDegerA1 < 0) {
      hedefDegerA1 = 0;
    }
  }
}

void ekranYaz(int analogDeger0, int analogDeger1, int dijitalDeger) {
  ekran.clearDisplay();

  ekran.setTextSize(1);
  ekran.setTextColor(SH110X_WHITE);

  ekran.setCursor(0, 0);
  ekran.print("Mod:");
  ekran.print(ayarModu);

  if (ayarModu == 0) {
    ekran.print(" AYAR YOK");
  }

  if (ayarModu == 1) {
    ekran.print(" A0 AYAR");
  }

  if (ayarModu == 2) {
    ekran.print(" A1 AYAR");
  }

  ekran.setCursor(0, 11);
  ekran.print("A0:");
  ekran.print(analogDeger0);
  ekran.print("/");
  ekran.print(hedefDegerA0);

  ekran.setCursor(90, 11);
  ekran.print("D7:");
  ekran.print(digitalRead(cikisPinA0) ? "H" : "L");

  ekran.setCursor(0, 22);
  ekran.print("A1:");
  ekran.print(analogDeger1);
  ekran.print("/");
  ekran.print(hedefDegerA1);

  ekran.setCursor(90, 22);
  ekran.print("D8:");
  ekran.print(digitalRead(cikisPinA1) ? "H" : "L");

  ekran.setCursor(0, 33);
  ekran.print("D6:");

  if (dijitalDeger == HIGH) {
    ekran.print("DOLU");
  } else {
    ekran.print("BOS");
  }

  ekran.setCursor(60, 33);
  ekran.print("N:");
  ekran.print(nem, 0);
  ekran.print("%");

  ekran.setCursor(0, 48);
  ekran.print("S:");
  ekran.print(sicaklik, 1);
  ekran.print("C");

  if (ayarModu == 1) {
    ekran.setCursor(70, 48);
    ekran.print("<A0>");
  }

  if (ayarModu == 2) {
    ekran.setCursor(70, 48);
    ekran.print("<A1>");
  }

  ekran.display();
}