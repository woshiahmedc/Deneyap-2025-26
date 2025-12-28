#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------
// Joystick
#define PIN_JOYSTICK_BTN D2
#define PIN_JOY_Y A0
#define PIN_JOY_X A1

// Servo Motorlar
#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// L298N Motor Sürücü Pinleri
#define L298N_ENA D3  // Motor A Hız (PWM)
#define L298N_IN1 D4  // Motor A Yön 1
#define L298N_IN2 D5  // Motor A Yön 2
#define L298N_IN3 D6  // Motor B Yön 1
#define L298N_IN4 D7  // Motor B Yön 2
#define L298N_ENB D8  // Motor B Hız (PWM)

// -------------------- SISTEM SABITLERI ----------------------
const int SERVO_MIN_US = 0;
const int SERVO_MAX_US = 3000;
const int JOY_ALT_ESIK = 1000;
const int JOY_UST_ESIK = 3000;

// -------------------- DURUM DEGISKENLERI --------------------
int anlikOkumaY, anlikOkumaX;
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;
int adimMiktari = 5;
int donguGecikmesi = 10;

Servo motor1;
Servo motor2;

// ============================================================
// SISTEM KURULUMU (SETUP)
// ============================================================
void setup() {
  // Servoları Başlat
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);

  // Joystick Buton
  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

  // L298N Pin Modları
  pinMode(L298N_ENA, OUTPUT);
  pinMode(L298N_IN1, OUTPUT);
  pinMode(L298N_IN2, OUTPUT);
  pinMode(L298N_IN3, OUTPUT);
  pinMode(L298N_IN4, OUTPUT);
  pinMode(L298N_ENB, OUTPUT);

  Serial.begin(115200);
  Serial.println("Sistem Baslatildi: Servo + L298N Kontrol Modu");
}

// ============================================================
// ANA DONGU (LOOP)
// ============================================================
void loop() {
  // 1. Verileri Oku
  anlikOkumaY = analogRead(PIN_JOY_Y);
  anlikOkumaX = analogRead(PIN_JOY_X);

  // 2. Servo Mantığını İşle
  servoKontrol();

  // 3. L298N Motor Sürücü Mantığını İşle
  motorSurucuKontrol();

  // 4. Çıkışları Uygula
  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  // Buton Kontrolü
  if (digitalRead(PIN_JOYSTICK_BTN) == LOW) {
    Serial.println("Joystick Butonu Aktif");
    delay(200);
  }

  delay(donguGecikmesi);
}

// ============================================================
// YARDIMCI FONKSIYONLAR
// ============================================================

void servoKontrol() {
  // Eksen 1 (Y) Servo Kontrolü
  if (anlikOkumaY < JOY_ALT_ESIK && hedefKonum1 > SERVO_MIN_US) {
    hedefKonum1 -= adimMiktari;
  } else if (anlikOkumaY > JOY_UST_ESIK && hedefKonum1 < SERVO_MAX_US) {
    hedefKonum1 += adimMiktari;
  }

  // Eksen 2 (X) Servo Kontrolü
  if (anlikOkumaX < JOY_ALT_ESIK && hedefKonum2 > SERVO_MIN_US) {
    hedefKonum2 -= adimMiktari;
  } else if (anlikOkumaX > JOY_UST_ESIK && hedefKonum2 < SERVO_MAX_US) {
    hedefKonum2 += adimMiktari;
  }
}

void motorSurucuKontrol() {
  int pwmHizi = 0;

  // Y ekseni üzerinden DC motor ileri/geri kontrolü
  if (anlikOkumaY > JOY_UST_ESIK) { 
    // İleri Hareket
    pwmHizi = map(anlikOkumaY, JOY_UST_ESIK, 4095, 0, 255);
    digitalWrite(L298N_IN1, HIGH);
    digitalWrite(L298N_IN2, LOW);
    digitalWrite(L298N_IN3, HIGH);
    digitalWrite(L298N_IN4, LOW);
  } 
  else if (anlikOkumaY < JOY_ALT_ESIK) { 
    // Geri Hareket
    pwmHizi = map(anlikOkumaY, JOY_ALT_ESIK, 0, 0, 255);
    digitalWrite(L298N_IN1, LOW);
    digitalWrite(L298N_IN2, HIGH);
    digitalWrite(L298N_IN3, LOW);
    digitalWrite(L298N_IN4, HIGH);
  } 
  else { 
    // Durdur (Deadzone içindeyse)
    pwmHizi = 0;
    digitalWrite(L298N_IN1, LOW);
    digitalWrite(L298N_IN2, LOW);
    digitalWrite(L298N_IN3, LOW);
    digitalWrite(L298N_IN4, LOW);
  }

  // Hız değerlerini uygula
  analogWrite(L298N_ENA, pwmHizi);
  analogWrite(L298N_ENB, pwmHizi);
}
