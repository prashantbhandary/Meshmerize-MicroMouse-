
// Download below two library on your arduino IDE you will get this from google
#include <SparkFun_TB6612.h>  // This libaray is for sparkfun motor driver  
#include <QTRSensors.h>       // It is for qtr sensors 

// Initialization of the motors
// #define AIN1 4
// #define BIN1 8
// #define AIN2 5
// #define BIN2 9

// #define PWMA 3
// #define PWMB 6
#define AIN1 5 //4
#define BIN1 8 //8
#define AIN2 4 //5
#define BIN2 9 //9

#define PWMA 3
#define PWMB 6

const int offsetA = 1;
const int offsetB = 1;

// Initialization of the controls
#define sw1 10
#define sw2 11
#define sw3 12 
#define led 7
int s1;

// Initialization of sensors
#define NUM_SENSORS 8
uint16_t sensors1[8];
int thr[8];

// Initialization of PID parameter
#define MaxSpeed 200
#define BaseSpeed 200
int lastError = 0;
float kp = 0.151;    // It fully depends on the bot system
float kd = 0.8;    // Please follow the method provided in instructables to get your values
// float ki=0.00000008;
int last_pos = 3500;


// Creating the instance of class for motor and senors
Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, 1); 
Motor motor2 = Motor(BIN1, BIN2, PWMB, 1, 1); // right

QTRSensors qtra;
void calibration()
{

  for (int i = 0; i <= 100; i++)
  {

    if (i < 25 || i >= 75)
    {
      left(motor1, motor2, 220); //Left turn 
    }
    else
    {
      right(motor1, motor2, 220); //Right turn
    }
    qtra.calibrate();
    delay(10);
  }  // end calibration cycle
  brake(motor1, motor2);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibrationOn.minimum[i]);
    Serial.print(' ');
    thr[i] = (qtra.calibrationOn.minimum[i] + qtra.calibrationOn.maximum[i]) / 2;
    // Calculating the threshold value for making the decision above thr black line and below white line
  }
  Serial.println();
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println(thr[0]);
  Serial.println(thr[7]);
 }

void follow_segment1()
{
    while(1){
    int position = qtra.readLineWhite(sensors1);       // FOR BLACK LINE FOLLOWER JUST REPLACE White WITH Black
    int error =  3500 - position;
    // Serial.println("position");
    // Serial.println(position);
    int motorSpeed = kp * error + kd * (error - lastError);
    lastError = error;
    int rightMotorSpeed = BaseSpeed - motorSpeed;
    int leftMotorSpeed = BaseSpeed + motorSpeed;
    if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed;
    if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed;
    if (rightMotorSpeed < 0)rightMotorSpeed = 0;
    if (leftMotorSpeed < 0)leftMotorSpeed = 0;
    // Serial.print("Right Motor Speed: ");
    // Serial.println(rightMotorSpeed);
    // Serial.print("Left Motor Speed: ");
    // Serial.println(leftMotorSpeed);
    motor1.drive(rightMotorSpeed);
    motor2.drive(leftMotorSpeed);


    delay(1);
    }
  }



void setup()
{
  qtra.setTypeAnalog();
  qtra.setSensorPins((const uint8_t[]) {   // QTR8 sensor setup
     A7 ,A6 , A5 ,A4 , A3 ,A2 , A1 , A0 
  }, NUM_SENSORS);

  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);

  Serial.begin(9600);
 

  s1 = digitalRead(sw1);
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);      // Calibration phase where the bot get calibrated after pressing s1 switch
  }
  delay(800);
  calibration();
  
}
void loop() {
  s1 = digitalRead(sw1);
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);      //s1 to start following line
  }
  delay(800);
  forward(motor1, motor2, 50);
  delay(40);
  forward(motor1, motor2, 70);
  delay(40);                              // For graduly increasing speed so that bot does not get direct high speed at the start
  forward(motor1, motor2, 90);
  delay(40);
  follow_segment1();
}
