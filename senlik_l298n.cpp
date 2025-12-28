// -------------------- PIN KONFIGURASYONU --------------------

// Joystick

#define PIN_JOYSTICK_BTN D12

#define PIN_JOY_Y A1   // Y ekseni → sağ-sol dönüş

#define PIN_JOY_X A0   // X ekseni → ileri-geri hareket

// L298N Motor Sürücü Pinleri

#define L298N_ENA D1  // Motor A Hız (PWM)

#define L298N_IN1 D4  // Motor A Yön 1

#define L298N_IN2 D15 // Motor A Yön 2

#define L298N_IN3 D8  // Motor B Yön 1

#define L298N_IN4 D9  // Motor B Yön 2

#define L298N_ENB D13 // Motor B Hız (PWM)

// -------------------- SABITLER --------------------

#define JOY_ALT_ESIK 1700

#define JOY_UST_ESIK 2300

// -------------------- DURUM DEGISKENLERI --------------------

int joyX, joyY;

int pwmA, pwmB;

// ============================================================

// SISTEM KURULUMU

// ============================================================

void setup() {

  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP);

  pinMode(L298N_ENA, OUTPUT);

  pinMode(L298N_IN1, OUTPUT);

  pinMode(L298N_IN2, OUTPUT);

  pinMode(L298N_IN3, OUTPUT);

  pinMode(L298N_IN4, OUTPUT);

  pinMode(L298N_ENB, OUTPUT);

  Serial.begin(115200);

  Serial.println("Sistem Baslatildi: XY Joystick Tank Kontrol");

}

// ============================================================

// ANA DONGU

// ============================================================

void loop() {

  // Joystick değerlerini oku

  joyX = analogRead(PIN_JOY_X);

  joyY = analogRead(PIN_JOY_Y);

  // Motorları kontrol et

  motorSurucuKontrol();

  // Joystick buton kontrolü

  if (digitalRead(PIN_JOYSTICK_BTN) == LOW) {

    Serial.println("Joystick Butonu Aktif");

    delay(200);

  }

  // Serial Debug: joystick + motor PWM

  Serial.print("JOY X: "); Serial.print(joyX);

  Serial.print(" | JOY Y: "); Serial.print(joyY);

  Serial.print(" | PWM A: "); Serial.print(pwmA);

  Serial.print(" | PWM B: "); Serial.println(pwmB);

  delay(20);

}

// ============================================================

// MOTOR SURUCU FONKSIYONU

// ============================================================

void motorSurucuKontrol() {

  pwmA = 0;

  pwmB = 0;

  // ---------------------------

  // X ekseni → ileri / geri

  // ---------------------------

  if (joyX > JOY_UST_ESIK) {

    pwmA = map(joyX, JOY_UST_ESIK, 4095, 0, 255);

    pwmB = pwmA;

    digitalWrite(L298N_IN1, HIGH);

    digitalWrite(L298N_IN2, LOW);

    digitalWrite(L298N_IN3, HIGH);

    digitalWrite(L298N_IN4, LOW);

  } 

  else if (joyX < JOY_ALT_ESIK) {

    pwmA = map(joyX, JOY_ALT_ESIK, 0, 0, 255);

    pwmB = pwmA;

    digitalWrite(L298N_IN1, LOW);

    digitalWrite(L298N_IN2, HIGH);

    digitalWrite(L298N_IN3, LOW);

    digitalWrite(L298N_IN4, HIGH);

  } 

  else {

    pwmA = 0;

    pwmB = 0;

    digitalWrite(L298N_IN1, LOW);

    digitalWrite(L298N_IN2, LOW);

    digitalWrite(L298N_IN3, LOW);

    digitalWrite(L298N_IN4, LOW);

  }

  // ---------------------------

  // Y ekseni → sola / sağa dönüş

  // ---------------------------

  if (joyY > JOY_UST_ESIK) {

    // Sağ dönüş → sol teker döner, sağ teker durur

    pwmA = map(joyY, JOY_UST_ESIK, 4095, 0, 255);

    pwmB = 0;

    digitalWrite(L298N_IN1, HIGH);

    digitalWrite(L298N_IN2, LOW);

    digitalWrite(L298N_IN3, LOW);

    digitalWrite(L298N_IN4, LOW);

  } 

  else if (joyY < JOY_ALT_ESIK) {

    // Sol dönüş → sağ teker döner, sol teker durur

    pwmA = 0;

    pwmB = map(joyY, JOY_ALT_ESIK, 0, 0, 255);

    digitalWrite(L298N_IN1, LOW);

    digitalWrite(L298N_IN2, LOW);

    digitalWrite(L298N_IN3, HIGH);

    digitalWrite(L298N_IN4, LOW);

  }

  // PWM değerlerini uygula

  analogWrite(L298N_ENA, pwmA);

  analogWrite(L298N_ENB, pwmB);

}

