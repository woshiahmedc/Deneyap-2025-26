#include <Deneyap_Servo.h>

// -------------------- PIN KONFIGURASYONU --------------------
// Joystick modülü üzerindeki buton bağlantısı.
// Test amaçlı kullanılacağı için harici direnç gerektirmeyen dahili pull-up kullanılacak.
#define PIN_JOYSTICK_BTN D14

// Joystick analog girişleri
#define PIN_JOY_X A0
#define PIN_JOY_Y A1

// Servo motor çıkışları
#define PIN_SERVO_1 D0
#define PIN_SERVO_2 D1

// -------------------- SISTEM SABITLERI ----------------------
// Servo motorların güvenli çalışma aralığı (mikrosaniye cinsinden).
// Mekanik sıkışmayı önlemek için bu değerler robotun fiziksel sınırlarına göre ayarlanmalıdır.
const int SERVO_MIN_US = 0;
const int SERVO_MAX_US = 3000;

// Analog okuma eşik değerleri (Deneyap 12-bit ADC: 0-4095).
// Orta nokta gürültüsünü (noise) filtrelemek için geniş bir ölü bölge (deadzone) bırakılmıştır.
const int JOY_ALT_ESIK = 1000;
const int JOY_UST_ESIK = 3000;

// -------------------- DURUM DEGISKENLERI --------------------
int anlikOkumaY, anlikOkumaX;

// Başlangıç pozisyonu orta nokta (1500us) olarak belirlenir.
// Bu, sistem açıldığında robotun ani ve tehlikeli bir hareket yapmasını engeller.
int hedefKonum1 = 1500;
int hedefKonum2 = 1500;

// Hareket hızı ayarları.
// 'adimMiktari': Her döngüde servonun ne kadar döneceği (ivmelenme kontrolü).
// 'donguGecikmesi': Döngü hızı (sistemin tepki süresi).
int adimMiktari = 5;
int donguGecikmesi = 10;

// -------------------- NESNE TANIMLARI -----------------------
Servo motor1;
Servo motor2;

// ============================================================
// SISTEM KURULUMU (SETUP)
// ============================================================
void setup() {
  // Servo motorları ilgili pinlere bağla
  motor1.attach(PIN_SERVO_1);
  motor2.attach(PIN_SERVO_2);

  // Buton için dahili pull-up direncini aktif et.
  // Bu sayede buton basılmadığında HIGH, basıldığında LOW okunur.
  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

  // Hata ayıklama ve test mesajları için seri portu başlat.
  Serial.begin(115200);
  Serial.println("Sistem Baslatildi: Manuel Kontrol Modu");
}

// ============================================================
// ANA DONGU (LOOP)
// ============================================================
void loop() {
  // 1. Joystick Verilerini İşle ve Konum Hesapla
  hareketVerileriniIsle();

  // 2. Buton Test Kontrolü
  // Kullanıcı bağlantıyı test etmek isterse butona basarak geri bildirim alabilir.
  if (digitalRead(PIN_JOYSTICK_BTN) == LOW) {
    Serial.println("Joystick algilandi");
    // Seri port ekranını mesajla doldurmamak (flood) için kısa bir bekleme eklenir.
    delay(200);
  }

  // 3. Motorları Sür
  // writeMicroseconds kullanımı, standart açı (0-180) komutuna göre daha hassas kontrol sağlar.
  motor1.writeMicroseconds(hedefKonum1);
  motor2.writeMicroseconds(hedefKonum2);

  // Sistemi stabil tutmak ve servo jitter'ını (titreme) önlemek için kısa bekleme.
  delay(donguGecikmesi);
}

// ============================================================
// YARDIMCI FONKSIYON: HAREKET MANTIGI
// ============================================================
void hareketVerileriniIsle() {
  // ADC'den ham verileri oku
  anlikOkumaY = analogRead(PIN_JOY_Y);
  anlikOkumaX = analogRead(PIN_JOY_X);

  // --- EKSEN 1 KONTROLÜ (Y) ---
  // Joystick aşağı çekildiyse ve minimum sınıra ulaşılmadıysa konumu azalt.
  if (anlikOkumaY < JOY_ALT_ESIK && hedefKonum1 > SERVO_MIN_US) {
    hedefKonum1 -= adimMiktari;
  }
  // Joystick yukarı itildiyse ve maksimum sınıra ulaşılmadıysa konumu artır.
  else if (anlikOkumaY > JOY_UST_ESIK && hedefKonum1 < SERVO_MAX_US) {
    hedefKonum1 += adimMiktari;
  }

  // --- EKSEN 2 KONTROLÜ (X) ---
  // Aynı mantık ikinci eksen için uygulanır.
  if (anlikOkumaX < JOY_ALT_ESIK && hedefKonum2 > SERVO_MIN_US) {
    hedefKonum2 -= adimMiktari;
  }
  else if (anlikOkumaX > JOY_UST_ESIK && hedefKonum2 < SERVO_MAX_US) {
    hedefKonum2 += adimMiktari;
  }
}
