/* Definisi Library,
   dan Inisialisasi Variabel */
#define analogPin1 1
#define analogPin2 2
#define analogPin3 3
#define resetButton 2
#define SENSOR_DISTANCE 30 // in cms
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int raw1 = 0;
int raw2 = 0;
int Vin = 5;
float Vout1 = 0;
float R1 = 0;
float Vout2 = 0;
float R2 = 0;
float temp = 0;
float timer;
float vel;
float traceTime[2];
float Rvar[2];
float R1_known = 95710; // in ohms
float R2_known = 103600; // in ohms
int state;
byte prev_button;

/* Inisialisasi state, LCD,
   Button, Serial Monitor untuk debugging  */
void setup()
{
  state = 0;
  lcd.init();
  lcd.backlight();
  pinMode(resetButton, INPUT_PULLUP);
  Serial.begin(9600);
}

/* Memulai loop */
void loop()
{

  // Mengambil data analog dari pin A1 dan A2
  raw1 = analogRead(analogPin1);
  raw2 = analogRead(analogPin2);

  // Reset Button handling
  if (!digitalRead(resetButton) && (prev_button == LOW))
  {
    state = 0;
    prev_button = HIGH;
  }
  if (digitalRead(resetButton)) {
    prev_button = LOW;
  }

  /* State User Interface, Menentukan apa saja yang harus
     dicetak pada LCD ketika alat berada pada state/kondisi tertentu */
  switch (state)
  {
    case 0: // Menunggu User menggerakan bola ke track/lintasan alat
      lcd.clear(); lcd.print("Waiting...");
      break;
    case 1: // Bola melewati sensor pertama
      lcd.clear(); lcd.print("Point 1 Passed...");
      break;
    case 2: // Bola melewati sensor kedua
      lcd.clear(); lcd.print("Point 1 Passed...");
      lcd.setCursor(0, 2); lcd.print("Point 2 Passed...");
      delay(1000);
      state = 3;
      break;
    case 3: // Setelah melewati sensor kedua, menghitung kecepatan
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calculate Velocity");
      delay(2000);
      lcd.print(".");
      delay(2000);
      state = 4;
      break;

    case 4: // Mencetak hasil kecepatan bola ke LCD, bergantian dg state 5
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Ball Velocity = ");
      lcd.setCursor(0, 1); lcd.print(vel); lcd.print(" cm/s");
      lcd.setCursor(0, 2); lcd.print("Rfotores1: "); lcd.print(Rvar[0]);
      lcd.setCursor(0, 3); lcd.print("Rfotores2: "); lcd.print(Rvar[1]);
      delay(2000);
      state = 5;
      break;
    case 5: // Mencetak saran press button untuk mengulangi pengukuran
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Ball Velocity = ");
      lcd.setCursor(0, 1); lcd.print(vel); lcd.print(" cm/s");
      lcd.setCursor(0, 2); lcd.print("Press Btn to Restart");
      delay(2000);
      state = 4;
  }

  // Main Algorithm
  if (raw1 || raw2)
  {
    // Menentukan R1 = Rpho1, dengan R1_known adalah nilai potentiometer
    temp = raw1 * Vin;
    Vout1 = temp / 1024;
    temp = (Vin / Vout1) - 1;
    R1 = R1_known * temp;
    timer = millis() ;

    // Apabila bola mengenai sensor 1, simpan nilai waktu ke array
    if (raw1 < 512 && (state == 0)) {
      state = 1;
      Serial.print("Point 1 Passed");
      Serial.print("          ");
      Serial.println(timer);
      traceTime[0] = timer;
      Rvar[0] = R1;
    }

    // Menentukan R2 = Rpho2, dengan R2_known adalah nilai potentiometer
    temp = raw2 * Vin;
    Vout2 = (temp) / 1024;
    temp = (Vin / Vout2) - 1;
    R2 = R2_known * temp;
    timer = millis();

    // Apabila bola mengenai sensor 2, simpan nilai waktu ke array
    if (raw2 < 512 && (state == 1)) {
      state = 2;
      Serial.print("Point 2 Passed");
      Serial.print("          ");
      Serial.println(timer);
      traceTime[1] = timer;
      Rvar[1] = R2;
    }

    vel = (SENSOR_DISTANCE * 1000) / (traceTime[1] - traceTime[0]);
    delay(10);

  }
}

void checkResistorValues() {
  Serial.print("raw1 : ");
  Serial.print (raw1);
  Serial.print("              raw2 : ");
  Serial.print(raw2);
  Serial.print("            R1: ");
  Serial.print(R1);
  Serial.print("            R2: ");
  Serial.println(R2);
}
