#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------

// --- JOYSTICK 1 (L298N / Sürüş Kontrolü) ---
// Kaynak dosyadaki tanımlar 
#define PIN_JOY1_BTN D12 
#define PIN_JOY1_Y A1    // Y ekseni -> sağ-sol dönüş 
#define PIN_JOY1_X A0    // X ekseni -> ileri-geri hareket 

// --- JOYSTICK 2 (Servo / Kol Kontrolü) ---
// Servo kontrolü için kullanılan ikinci joystick (Pinler değişmedi)
#define PIN_JOY2_Y A2    // Servo 2 Kontrolü
#define PIN_JOY2_X A3    // Servo 1 Kontrolü

// --- SERVO MOTORLAR ---
#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// --- L298N MOTOR SÜRÜCÜ ---
// 'senlik_l298n_truepins.txt' dosyasından alınan doğru pinler 
#define L298N_ENA D1  // Motor A Hız (PWM)
#define L298N_IN1 D4  // Motor A Yön 1
#define L298N_IN2 D15 // Motor A Yön 2
#define L298N_IN3 D8  // Motor B Yön 1
#define L298N_IN4 D9  // Motor B Yön 2
#define L298N_ENB D13 // Motor B Hız (PWM)

// -------------------- SISTEM SABITLERI ----------------------
const int SERVO_MIN_US = 0;
const int SERVO_MAX_US = 3000;

// Joystickler için ölü bölge (deadzone) 
const int JOY_ALT_ESIK = 1700; // Kaynak dosyadan alınan yeni eşik değerleri
const int JOY_UST_ESIK = 2300; // Kaynak dosyadan alınan yeni eşik değerleri

// -------------------- DURUM DEGISKENLERI --------------------
int joy1_Y, joy1_X;
int joy2_Y, joy2_X;

// Servo Hedef Konumları
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;

// Hız Ayarları
int adimMiktari = 5;      
int donguGecikmesi = 10; 

Servo motor1;
Servo motor2;

// ============================================================
// SISTEM KURULUMU (SETUP)
// ============================================================
void setup() {
  // Servo Motorlar
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);

  // Joystick 1 Buton [cite: 29]
  pinMode(PIN_JOY1_BTN, INPUT_PULLUP);
  
  // L298N Pinleri [cite: 29, 30]
  pinMode(L298N_ENA, OUTPUT);
  pinMode(L298N_IN1, OUTPUT);
  pinMode(L298N_IN2, OUTPUT);
  pinMode(L298N_IN3, OUTPUT);
  pinMode(L298N_IN4, OUTPUT);
  pinMode(L298N_ENB, OUTPUT);

  Serial.begin(115200);
  Serial.println("Sistem Baslatildi: XY Joystick Tank + Servo Kontrol");
}

// ============================================================
// ANA DONGU (LOOP)
// ============================================================
void loop() {
  // 1. Veri Okuma [cite: 31]
  joy1_X = analogRead(PIN_JOY1_X); // İleri-Geri
  joy1_Y = analogRead(PIN_JOY1_Y); // Sağ-Sol
  
  joy2_Y = analogRead(PIN_JOY2_Y);
  joy2_X = analogRead(PIN_JOY2_X);

  // 2. Servo Kontrol (Joystick 2)
  servoHareketHesapla();

  // 3. Motor Sürüş Kontrol (Joystick 1) [cite: 32]
  motorSurucuKontrol();

  // 4. Çıkışları Uygula (Servo)
  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  // Buton Kontrolü [cite: 32]
  if (digitalRead(PIN_JOY1_BTN) == LOW) {
    Serial.println("Joystick Butonu Aktif - Reset");
    hedefKonum1 = 1500;
    hedefKonum2 = 1500;
    delay(200); 
  }

  delay(donguGecikmesi);
}

// ============================================================
// YARDIMCI FONKSIYONLAR
// ============================================================

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
  int pwmA = 0;
  int pwmB = 0;

  // X ekseni -> İleri / Geri [cite: 36]
  if (joy1_X > JOY_UST_ESIK) {
    pwmA = map(joy1_X, JOY_UST_ESIK, 4095, 0, 255);
    pwmB = pwmA; // İleri giderken iki motor da eşit hızda [cite: 37]
    
    digitalWrite(L298N_IN1, HIGH);
    digitalWrite(L298N_IN2, LOW);
    digitalWrite(L298N_IN3, HIGH);
    digitalWrite(L298N_IN4, LOW);
  } 
  else if (joy1_X < JOY_ALT_ESIK) {
    pwmA = map(joy1_X, JOY_ALT_ESIK, 0, 0, 255);
    pwmB = pwmA; // Geri giderken iki motor da eşit hızda [cite: 39]
    
    digitalWrite(L298N_IN1, LOW);
    digitalWrite(L298N_IN2, HIGH);
    digitalWrite(L298N_IN3, LOW);
    digitalWrite(L298N_IN4, HIGH);
  } 
  else {
    // Y ekseni -> Sola / Sağa Dönüş (Tank Dönüşü Değil, Tek Teker Hareketi) 
    
    if (joy1_Y > JOY_UST_ESIK) {
      // Sağ Dönüş -> Sol teker (A) döner, Sağ teker (B) durur [cite: 41, 42]
      pwmA = map(joy1_Y, JOY_UST_ESIK, 4095, 0, 255);
      pwmB = 0;

      digitalWrite(L298N_IN1, HIGH);
      digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, LOW);
      digitalWrite(L298N_IN4, LOW);
    } 
    else if (joy1_Y < JOY_ALT_ESIK) {
      // Sol Dönüş -> Sağ teker (B) döner, Sol teker (A) durur [cite: 43, 44]
      pwmA = 0;
      pwmB = map(joy1_Y, JOY_ALT_ESIK, 0, 0, 255);

      digitalWrite(L298N_IN1, LOW);
      digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, HIGH);
      digitalWrite(L298N_IN4, LOW);
    }
    else {
      // Tam Durma
      pwmA = 0;
      pwmB = 0;
      digitalWrite(L298N_IN1, LOW);
      digitalWrite(L298N_IN2, LOW);
      digitalWrite(L298N_IN3, LOW);
      digitalWrite(L298N_IN4, LOW);
    }
  }

  // PWM Değerlerini Yaz [cite: 45]
  analogWrite(L298N_ENA, pwmA);
  analogWrite(L298N_ENB, pwmB);
}
