#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------

// --- JOYSTICK 1 (L298N / Sürüş Kontrolü) ---
#define PIN_JOY1_BTN D12 
#define PIN_JOY1_Y A1    
#define PIN_JOY1_X A0    

// --- JOYSTICK 2 (Servo / Kol Kontrolü) ---
#define PIN_JOY2_Y A2    
#define PIN_JOY2_X A3    

// --- SERVO MOTORLAR ---
#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// --- L298N MOTOR SÜRÜCÜ ---
#define L298N_ENA D1     
#define L298N_IN1 D4     
#define L298N_IN2 D15    
#define L298N_IN3 D8     
#define L298N_IN4 D9     
#define L298N_ENB D13    

// -------------------- SISTEM SABITLERI ----------------------
const int SERVO_MIN_US = 0;
const int SERVO_MAX_US = 3000;

// Joystickler için ölü bölge (deadzone)
const int JOY_ALT_ESIK = 1700;
const int JOY_UST_ESIK = 2300;

// -------------------- DURUM DEGISKENLERI --------------------
// Joystick Verileri
int joy1_Y, joy1_X;
int joy2_Y, joy2_X;

// Servo Hedef Konumları
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;

// Motor PWM Değerleri (Global yapıldı, debug için)
int pwmA = 0;
int pwmB = 0;

// Hız Ayarları
int adimMiktari = 5;
int donguGecikmesi = 50; // Debug okunabilsin diye biraz artırıldı

Servo motor1;
Servo motor2;

// ============================================================
// SISTEM KURULUMU (SETUP)
// ============================================================
void setup() {
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);

  pinMode(PIN_JOY1_BTN, INPUT_PULLUP);
  
  pinMode(L298N_ENA, OUTPUT);
  pinMode(L298N_IN1, OUTPUT);
  pinMode(L298N_IN2, OUTPUT);
  pinMode(L298N_IN3, OUTPUT);
  pinMode(L298N_IN4, OUTPUT);
  pinMode(L298N_ENB, OUTPUT);

  Serial.begin(115200);
  Serial.println("Sistem Baslatildi: XY Joystick + Servo + Debug Modu");
  delay(1000);
}

// ============================================================
// ANA DONGU (LOOP)
// ============================================================
void loop() {
  // 1. Veri Okuma
  joy1_X = analogRead(PIN_JOY1_X);
  joy1_Y = analogRead(PIN_JOY1_Y);
  joy2_Y = analogRead(PIN_JOY2_Y);
  joy2_X = analogRead(PIN_JOY2_X);

  // 2. Servo Hareket Hesapla
  servoHareketHesapla();

  // 3. Motor Sürüş Hesapla
  motorSurucuKontrol();

  // 4. Çıkışları Uygula (Servo)
  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  // 5. DEBUG YAZDIRMA (Tüm veriler tek satırda)
  debugYazdir();

  // Buton Kontrolü (Reset)
  if (digitalRead(PIN_JOY1_BTN) == LOW) {
    Serial.println("!!! BUTON BASILDI - KONUMLAR SIFIRLANIYOR !!!");
    hedefKonum1 = 1500;
    hedefKonum2 = 1500;
    delay(500); 
  }

  delay(donguGecikmesi);
}

// ============================================================
// YARDIMCI FONKSIYONLAR
// ============================================================

void debugYazdir() {
  // Joystick 1 (Sürüş)
  Serial.print("J1_X:"); Serial.print(joy1_X);
  Serial.print("\tJ1_Y:"); Serial.print(joy1_Y);
  
  // Joystick 2 (Servo)
  Serial.print("\t| J2_X:"); Serial.print(joy2_X);
  Serial.print("\tJ2_Y:"); Serial.print(joy2_Y);

  // Servo Konumları
  Serial.print("\t| S1:"); Serial.print(hedefKonum1);
  Serial.print("\tS2:"); Serial.print(hedefKonum2);

  // Motor PWM
  Serial.print("\t| MA:"); Serial.print(pwmA);
  Serial.print("\tMB:"); Serial.println(pwmB);
}

void servoHareketHesapla() {
  // Servo 1 (X Ekseni - Joystick 2)
  if (joy2_X < JOY_ALT_ESIK && hedefKonum1 > SERVO_MIN_US) {
    hedefKonum1 -= adimMiktari;
  } else if (joy2_X > JOY_UST_ESIK && hedefKonum1 < SERVO_MAX_US) {
    hedefKonum1 += adimMiktari;
  }

  // Servo 2 (Y Ekseni - Joystick 2)
  if (joy2_Y < JOY_ALT_ESIK && hedefKonum2 > SERVO_MIN_US) {
    hedefKonum2 -= adimMiktari;
  } else if (joy2_Y > JOY_UST_ESIK && hedefKonum2 < SERVO_MAX_US) {
    hedefKonum2 += adimMiktari;
  }
}

void motorSurucuKontrol() {
  // Reset PWM varsayılanı
  pwmA = 0;
  pwmB = 0;

  // X ekseni -> İleri / Geri 
  if (joy1_X > JOY_UST_ESIK) {
    pwmA = map(joy1_X, JOY_UST_ESIK, 4095, 0, 255);
    pwmB = pwmA; 
    
    digitalWrite(L298N_IN1, HIGH); digitalWrite(L298N_IN2, LOW);
    digitalWrite(L298N_IN3, HIGH); digitalWrite(L298N_IN4, LOW);
  } 
  else if (joy1_X < JOY_ALT_ESIK) {
    pwmA = map(joy1_X, JOY_ALT_ESIK, 0, 0, 255);
    pwmB = pwmA; 
    
    digitalWrite(L298N_IN1, LOW); digitalWrite(L298N_IN2, HIGH);
    digitalWrite(L298N_IN3, LOW); digitalWrite(L298N_IN4, HIGH);
  } 
  else {
    // Y ekseni -> Sola / Sağa Dönüş
    if (joy1_Y > JOY_UST_ESIK) { // SAĞ
      pwmA = map(joy1_Y, JOY_UST_ESIK, 4095, 0, 255);
      pwmB = 0;

      digitalWrite(L298N_IN1, HIGH); digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, LOW);  digitalWrite(L298N_IN4, LOW);
    } 
    else if (joy1_Y < JOY_ALT_ESIK) { // SOL
      pwmA = 0;
      pwmB = map(joy1_Y, JOY_ALT_ESIK, 0, 0, 255);

      digitalWrite(L298N_IN1, LOW);  digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, HIGH); digitalWrite(L298N_IN4, LOW);
    }
    else {
      // DURMA (Zaten varsayılan 0)
      digitalWrite(L298N_IN1, LOW); digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, LOW); digitalWrite(L298N_IN4, LOW);
    }
  }

  // PWM Değerlerini Yaz 
  analogWrite(L298N_ENA, pwmA);
  analogWrite(L298N_ENB, pwmB);
}