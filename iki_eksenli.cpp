#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------
#define PIN_JOYSTICK_BTN D12

#define PIN_JOY_Y A0
#define PIN_JOY_X A1

#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// -------------------- SISTEM SABITLERI ----------------------
// Servo motorlar için güvenli mikro-saniye aralığı
const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2500;

// Deneyap ADC 12-bit: 0 - 4095
// Geniş deadzone
const int JOY_ALT_ESIK = 1000;
const int JOY_UST_ESIK = 3000;

// -------------------- DURUM DEGISKENLERI --------------------
int anlikOkumaY = 0;
int anlikOkumaX = 0;

// Başlangıç konumu (orta nokta)
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;

// Hareket ayarları
int adimMiktari = 5;
int donguGecikmesi = 10;

// -------------------- NESNE TANIMLARI -----------------------
Servo motor1;
Servo motor2;

// ============================================================
// SETUP
// ============================================================
void setup() {
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);

  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Sistem Baslatildi - Joystick Manuel Kontrol");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  hareketVerileriniIsle();

  // Buton testi
  if (digitalRead(PIN_JOYSTICK_BTN) == LOW) {
    Serial.println("Joystick butonu BASILI");
    delay(200);
  }

  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  delay(donguGecikmesi);
}

// ============================================================
// JOYSTICK OKUMA VE HAREKET MANTIGI
// ============================================================
void hareketVerileriniIsle() {
  anlikOkumaY = analogRead(PIN_JOY_Y);
  anlikOkumaX = analogRead(PIN_JOY_X);

  // --- Y EKSENI (Servo 1) ---
  if (anlikOkumaY < JOY_ALT_ESIK) {
    hedefKonum1 -= adimMiktari;
  }
  else if (anlikOkumaY > JOY_UST_ESIK) {
    hedefKonum1 += adimMiktari;
  }

  // --- X EKSENI (Servo 2) ---
  if (anlikOkumaX < JOY_ALT_ESIK) {
    hedefKonum2 -= adimMiktari;
  }
  else if (anlikOkumaX > JOY_UST_ESIK) {
    hedefKonum2 += adimMiktari;
  }

  // Servo sınır koruması
  hedefKonum1 = constrain(hedefKonum1, SERVO_MIN_US, SERVO_MAX_US);
  hedefKonum2 = constrain(hedefKonum2, SERVO_MIN_US, SERVO_MAX_US);

  // -------- SERIAL MONITOR ÇIKTISI --------
  Serial.print("JOY X: ");
  Serial.print(anlikOkumaX);
  Serial.print(" | JOY Y: ");
  Serial.print(anlikOkumaY);
  Serial.print(" || SERVO1: ");
  Serial.print(hedefKonum1);
  Serial.print(" us | SERVO2: ");
  Serial.print(hedefKonum2);
  Serial.println(" us");
}
