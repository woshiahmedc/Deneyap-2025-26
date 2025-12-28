#include <ESP32Servo.h>
#include "BluetoothSerial.h"

// Bluetooth nesnesi (Sürüş için aktif bırakıldı)
BluetoothSerial SerialBT;

// Servo Nesneleri
Servo servoOmuz;  // Servo 2 ve 3 (Paralel)
Servo servoPence; // Servo 4

// Pin Tanımlamaları
const int joyX = A0;
const int joyY = A1;
const int joySW = D4;

const int solIleri = D0;
const int solGeri  = D1;
const int sagIleri = D8;
const int sagGeri  = D9;

// Açı Değişkenleri ve Sınırlar
int omuzAci = 90;
int penceAci = 90;
const int servoHiz = 2; // Hareket yumuşaklığı (Artırırsan hızlanır)

void setup() {
  // Motor pinleri
  pinMode(solIleri, OUTPUT);
  pinMode(solGeri, OUTPUT);
  pinMode(sagIleri, OUTPUT);
  pinMode(sagGeri, OUTPUT);

  // Joystick Buton (D4)
  pinMode(joySW, INPUT_PULLUP);

  // Servoları bağla
  servoOmuz.attach(D13);
  servoPence.attach(D14);

  // Başlangıç pozisyonları
  servoOmuz.write(omuzAci);
  servoPence.write(penceAci);

  // Haberleşme
  SerialBT.begin("Deneyap_Robot_Araba");
  Serial.begin(115200);
}

void loop() {
  // --- 1. JOYSTICK İLE KOL KONTROLÜ ---
  int xDeger = analogRead(joyX);
  int yDeger = analogRead(joyY);

  // Omuz Kontrolü (VRX)
  if (xDeger > 3000) {
    omuzAci = min(180, omuzAci + servoHiz);
  } else if (xDeger < 1000) {
    omuzAci = max(0, omuzAci - servoHiz);
  }
  servoOmuz.write(omuzAci);

  // Pençe Kontrolü (VRY)
  if (yDeger > 3000) {
    penceAci = min(180, penceAci + servoHiz);
  } else if (yDeger < 1000) {
    penceAci = max(0, penceAci - servoHiz);
  }
 
  // SW Butonu (D4) - Basıldığında Pençeyi Hızla Kapat (0 derece)
  if (digitalRead(joySW) == LOW) {
    penceAci = 0;
  }
 
  servoPence.write(penceAci);

  // --- 2. BLUETOOTH İLE SÜRÜŞ KONTROLÜ ---
  if (SerialBT.available()) {
    char komut = SerialBT.read();
    switch (komut) {
      case 'F': ileriGit(); break;
      case 'B': geriGit();  break;
      case 'L': solDon();   break;
      case 'R': sagDon();   break;
      case 'S': dur();      break;
    }
  }
 
  delay(15); // İşlemciyi rahatlatmak ve akıcı hareket için
}

// --- Hareket Fonksiyonları ---
void ileriGit() {
  digitalWrite(solIleri, HIGH); digitalWrite(solGeri, LOW);
  digitalWrite(sagIleri, HIGH); digitalWrite(sagGeri, LOW);
}

void geriGit() {
  digitalWrite(solIleri, LOW);  digitalWrite(solGeri, HIGH);
  digitalWrite(sagIleri, LOW);  digitalWrite(sagGeri, HIGH);
}

void solDon() {
  digitalWrite(solIleri, LOW);  digitalWrite(solGeri, HIGH);
  digitalWrite(sagIleri, HIGH); digitalWrite(sagGeri, LOW);
}

void sagDon() {
  digitalWrite(solIleri, HIGH); digitalWrite(solGeri, LOW);
  digitalWrite(sagIleri, LOW);  digitalWrite(sagGeri, HIGH);
}

void dur() {
  digitalWrite(solIleri, LOW);  digitalWrite(solGeri, LOW);
  digitalWrite(sagIleri, LOW);  digitalWrite(sagGeri, LOW);
}
