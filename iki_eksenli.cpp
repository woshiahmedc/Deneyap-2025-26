#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------
#define PIN_JOYSTICK_BTN D14
#define PIN_JOY_X A0
#define PIN_JOY_Y A1
#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// -------------------- SISTEM SABITLERI ----------------------
// GÜVENLİK REVİZYONU 1: Sınır değerleri güvenli aralığa çekildi.
// 0 ve 3000 değerleri servoları yakabilir. Standart güvenli aralık 1000-2000'dir.
const int SERVO_MIN_US = 1000; 
const int SERVO_MAX_US = 2000; 

// Analog okuma eşik değerleri
const int JOY_ALT_ESIK = 1000;
const int JOY_UST_ESIK = 3000;

// -------------------- DURUM DEGISKENLERI --------------------
int anlikOkumaY, anlikOkumaX;
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;

// Hız ayarları
int adimMiktari = 5;
int donguGecikmesi = 10;

// -------------------- NESNE TANIMLARI -----------------------
Servo motor1;
Servo motor2;

// ============================================================
// SISTEM KURULUMU (SETUP)
// ============================================================
void setup() {
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);
  
  // Başlangıçta motorları güvenli orta noktaya al
  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Sistem Baslatildi: Veri Okuma Modu Aktif");
  Serial.println("Joy_X \t Joy_Y \t Servo_1 \t Servo_2");
}

// ============================================================
// ANA DONGU (LOOP)
// ============================================================
void loop() {
  // 1. Joystick Verilerini İşle ve Konum Hesapla
  hareketVerileriniIsle();

  // 2. Buton Test Kontrolü
  if (digitalRead(PIN_JOYSTICK_BTN) == LOW) {
    Serial.print("--- BUTONA BASILDI --- ");
    delay(50); // Debounce süresi 10ms'den 50ms'ye çıkarıldı (Parazit önleme)
  }

  // 3. Motorları Sür
  // GÜVENLİK REVİZYONU 2: 'constrain' ile donanımsal sınırları garanti altına alıyoruz.
  // Hesaplama hatası olsa bile motora sınır dışı değer gitmez.
  hedefKonum1 = constrain(hedefKonum1, SERVO_MIN_US, SERVO_MAX_US);
  hedefKonum2 = constrain(hedefKonum2, SERVO_MIN_US, SERVO_MAX_US);

  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  // ------------------------------------------------------------
  // SERİ EKRANA YAZDIRMA
  // ------------------------------------------------------------
  Serial.print("JoyX:"); Serial.print(anlikOkumaX);
  Serial.print("\t JoyY:"); Serial.print(anlikOkumaY);
  Serial.print("\t Srv1:"); Serial.print(hedefKonum1);
  Serial.print("\t Srv2:"); Serial.println(hedefKonum2);
  // ------------------------------------------------------------

  delay(donguGecikmesi);
}

// ============================================================
// YARDIMCI FONKSIYON: HAREKET MANTIGI
// ============================================================
void hareketVerileriniIsle() {
  anlikOkumaY = analogRead(PIN_JOY_Y);
  anlikOkumaX = analogRead(PIN_JOY_X);

  // --- EKSEN 1 KONTROLÜ (Y) ---
  if (anlikOkumaY < JOY_ALT_ESIK) { // && kontrolüne gerek kalmadı, constrain kullanıyoruz
    hedefKonum1 -= adimMiktari;
  }
  else if (anlikOkumaY > JOY_UST_ESIK) {
    hedefKonum1 += adimMiktari;
  }

  // --- EKSEN 2 KONTROLÜ (X) ---
  if (anlikOkumaX < JOY_ALT_ESIK) {
    hedefKonum2 -= adimMiktari;
  }
  else if (anlikOkumaX > JOY_UST_ESIK) {
    hedefKonum2 += adimMiktari;
  }
}
