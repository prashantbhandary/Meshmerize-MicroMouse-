#include <QTRSensors.h>
// Sensors
#define NUM_SENSORS 8
uint16_t sensors1[NUM_SENSORS];  // Changed from unsigned int to uint16_t
int thr[NUM_SENSORS];

QTRSensors qtra;
    
// #define sw1 26  
// #define sw2 27  
// #define led 23  
// int s1, s2;

void setup() {
  Serial.begin(9600); 

  // Initialize QTR sensor pins (valid ADC pins for ESP32)
  qtra.setTypeAnalog();
  qtra.setSensorPins((const uint8_t[]) {
    // A0, A1, A2, A3, A4, A5, A6, A7 // Ensure these are connected to QTR sensors
    A7 ,A6 , A5 ,A4 , A3 ,A2 ,A1 ,A0
  }, NUM_SENSORS);

  // Pin mode setup (if you need to use switches or LEDs)
  // pinMode(sw1, INPUT);  // Using internal pull-up resistors
  // pinMode(sw2, INPUT);
  // pinMode(led, OUTPUT);

  // s1 = digitalRead(sw1);  //s1 for calibration
  // while (s1 == HIGH)
  // {
  //   s1 = digitalRead(sw1);   

  // }
  Serial.println("switch clicked");
  // digitalWrite(led,HIGH);
  delay(800);
  calibration();
  // digitalWrite(led,LOW);
}

void loop() {
  see_error();
}

void calibration() {
  for (int i = 0; i <= 100; i++) {
    qtra.calibrate();
    delay(10);
  }

  Serial.println("Calibration Min Values:");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(qtra.calibrationOn.minimum[i]);
    Serial.print(' ');
    thr[i] = (qtra.calibrationOn.minimum[i] + qtra.calibrationOn.maximum[i]) / 2;
  }
  Serial.println();

  Serial.println("Calibration Max Values:");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(qtra.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();

  Serial.println("Threshold Values:");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(thr[i]);
    Serial.print(' ');
  }
  Serial.println();

  Serial.println("Calibration complete. Press any key in the Serial Monitor to continue...");
  while (!Serial.available()) {
    // Wait for serial input
  }
  Serial.read();
}

void see_error() {
  Serial.println("Sensor Values:");
  
  // Read the sensor values
  qtra.read(sensors1);  // Uses QTRSensors to read the values

  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensors1[i]);
    Serial.print(' ');
  }
  Serial.println();

  // Present position of bot
  int position = qtra.readLineWhite(sensors1);  // Uses the corrected data type
  int error = 3500 - position;

  Serial.print("Position: ");
  Serial.println(position);
  Serial.print("Error: ");
  Serial.println(error);
  delay(400);
}
