
// Download below two library on your arduino IDE you will get this from google
#include <SparkFun_TB6612.h>  // This libaray is for sparkfun motor driver  
#include <QTRSensors.h>       // It is for qtr sensors 

// Initialization of the motors
#define AIN1 18
#define BIN1 16
#define AIN2 5
#define BIN2 17
#define PWMA 19
#define PWMB 4



const int offsetA = 1;
const int offsetB = 1;

// Initialization of the controls
#define sw1 26
#define sw2 27
#define led 23
int s1;




// Initialization of sensors
#define NUM_SENSORS 8
uint16_t sensors1[8];
int thr[8];

// Initialization of PID parameter
#define MaxSpeed 130
#define BaseSpeed 130
int lastError = 0;
float kp = 0.05;    // It fully depends on the bot system
float kd = 0.1;    // Please follow the method provided in instructables to get your values
int last_pos = 3500;




// Creating the instance of class for motor and senors
Motor motor2 = Motor(BIN1, BIN2, PWMB, 1, 1);
Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, 1); //right

QTRSensors qtra;



void calibration()
{

  for (int i = 0; i <= 100; i++)
  {

    if (i < 25 || i >= 75)
    {
      left(motor1, motor2, 180); //Left turn

    }
    else
    {

      right(motor1, motor2, 180); //Right turn
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

    
    motor1.drive(leftMotorSpeed);
    motor2.drive(rightMotorSpeed);


    delay(1);
    }
  }



void setup()
{
  qtra.setTypeAnalog();
  qtra.setSensorPins((const uint8_t[]) {   // QTR8 sensor setup
    15,25,33,32,35,34,39,36
  }, NUM_SENSORS);

  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);


  Serial.begin(115200);
 

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
  // forward(motor1, motor2, 60);
  // delay(40);
  // forward(motor1, motor2, 80);
  // delay(40);                              // For graduly increasing speed so that bot does not get direct high speed at the start
  // forward(motor1, motor2, 100);
  // delay(40);
  follow_segment1();
}
