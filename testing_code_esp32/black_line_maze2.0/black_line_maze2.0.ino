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

int num = 0;
char path[100];
int path_length = 0;

// Initialization of PID parameter
#define MaxSpeed 150
#define BaseSpeed 150
int lastError = 0;
float kp = 0.058;    // It fully depends on the bot system
float kd = 0.1;    // Please follow the method provided in instructables to get your values
int last_pos = 3500;

char dir;
int chr = 1;

// Creating the instance of class for motor and senors
Motor motor1 = Motor(BIN1, BIN2, PWMB, 1, 1);
Motor motor2 = Motor(AIN1, AIN2, PWMA, 1, 1); //right

QTRSensors qtra;

void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  qtra.setTypeAnalog();
  qtra.setSensorPins((const uint8_t[]) {   // QTR8 sensor setup
    15,25,33,32,35,34,39,36
  }, NUM_SENSORS);

  pinMode(sw1, INPUT);
  analogReadResolution(10);
 
  s1 = digitalRead(sw1);
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);      // Calibration phase where the bot get calibrated after pressing s1 switch
  }
  
  delay(800);
  calibration();
  digitalWrite(led,LOW);
  
}
void loop() {
  s1 = digitalRead(sw1);
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);      //s1 to start following line
  }
  delay(800);
  forward(motor1, motor2, 60);
  delay(40);
  forward(motor1, motor2, 80);
  delay(40);                              // For graduly increasing speed so that bot does not get direct high speed at the start
  forward(motor1, motor2, 100);
  delay(40);
  maze();
}

void calibration()
{
  digitalWrite(led,HIGH);
  for (int i = 0; i <= 100; i++)
  {

    if (i < 25 || i >= 75)
    {
      left(motor1, motor2, 200); //Left turn

    }
    else
    {
      right(motor1, motor2, 200); //Right turn
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

void maze()
{
  while (1)
  {
    follow_Segment(); // It will follow the path until it dosn't get any intersection
    digitalWrite(led, HIGH);
    // Serial.println("intersection");

    brake(motor1, motor2);           // After getting intersetion first thing that we have to do is
    // forward(motor1, motor2, 30);
    // delay(50);
    // brake(motor1, motor2);

    // digitalWrite(led,HIGH);
    
    unsigned char found_left = 0;
    unsigned char found_straight = 0;
    unsigned char found_right = 0;

    // Now read the sensors and check the intersection type.
    qtra.readLineBlack(sensors1);
    // if (sensors1[7] < thr[7])
    // {
    //   found_right = 1;
    //   if (sensors1[1] < thr[1])
    //   {
    //     found_left = 1;
    //   }
    // }
    // if (sensors1[0] < thr[0])
    // {
    //   found_left = 1;
    //   if (sensors1[6] < thr[6])
    //   {
    //     found_right = 1;

    //   }
    // }
    if(sensors1[0] > thr[0]){
    found_left = 1;
    }
    
    if(sensors1[7] > thr[7]){
    found_right = 1;
    }


    // Drive straight a bit more - this is enough to line up our
    //wheels with the intersection.
    // Serial.print("found_left");
    // Serial.println(found_left);
    // Serial.print("found_lright");
    // Serial.println(found_right); 

    forward(motor1, motor2, 50);
    delay(60);
    brake(motor1, motor2);
    qtra.readLineBlack(sensors1);
    if (sensors1[1] > thr[1] || sensors1[2] > thr[2] || sensors1[3] > thr[3] || sensors1[4] > thr[4] || sensors1[5] > thr[5] || sensors1[6] > thr[6] )
    {
       found_straight = 1;
      Serial.print("found_straight");
      // Serial.println(found_straight);
    }
    // Check for the ending spot.
    if (sensors1[1] > thr[1] && sensors1[2] > thr[2] && sensors1[3] > thr[3] && sensors1[4] > thr[4] && sensors1[5] > thr[5] && sensors1[6] > thr[6]){
     Serial.println("endpoint");
      break;
    }
    // According to rule we have to decided which turn has to be taken
    if (chr == 1)
      dir = select_turnL(found_left, found_straight, found_right);
    else if (chr == 2)
      dir = select_turnR(found_right, found_straight, found_left);  
    Serial.println(dir);
    // // Take a turn according to that
    turn(dir);
    //  Serial.println(dir);
    path[path_length] = dir;
    path_length++;
    simplify_path();
  }

  brake(motor1, motor2);
  forward(motor1, motor2, 80);
  delay(400);                   // Move straight a bit on end point and glow a led
  brake(motor1, motor2);

  for (int w = 0; w < path_length; w++)
  {
    Serial.print(path[w]);
    Serial.print(' ');
  }

  digitalWrite(led, HIGH);
  delay(4000);
  digitalWrite(led, LOW);

  s1 = digitalRead(sw1);
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);     // Wait for pressing a switch
  }

  delay(800);
  forward(motor1, motor2, 60);
  delay(40);
  forward(motor1, motor2, 80);
  delay(40);
  forward(motor1, motor2, 100);
  delay(40);
  while (1)
  {
    int k;
    for (k = 0; k < path_length; k++)
    {
          follow_Segment();
      forward(motor1, motor2, 180);    // After reaching a intercetion follow the shortest path turn
      delay(50);
      forward(motor1, motor2, 180);
      delay(200); 
      brake(motor1, motor2);
      delay(5);
      turn(path[k]);
    }
   follow_Segment();
    brake(motor1, motor2);
    forward(motor1, motor2, 200);
    delay(400);
    brake(motor1, motor2);
    digitalWrite(led, HIGH);
    delay(4000);
    digitalWrite(led, LOW);
  }
  }

void follow_Segment()
{
  while (1)
  {
    digitalWrite(led, LOW);
    // Serial.print("inside FOLLOW SEGMENT");
    // FOR BLACK LINE FOLLOWER JUST REPLACE White WITH Black
    int position = qtra.readLineBlack(sensors1);   // Getting the present position of the bot
    int error =  3500 - position;
    int motorSpeed = kp * error + kd * (error - lastError);
    lastError = error;
    Serial.println(position);
    Serial.println(error);
    int rightMotorSpeed = BaseSpeed - motorSpeed;
    int leftMotorSpeed = BaseSpeed + motorSpeed;
    if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed;
    if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed;
    if (rightMotorSpeed < 0)rightMotorSpeed = 0;
    if (leftMotorSpeed < 0)leftMotorSpeed = 0;
    Serial.println(rightMotorSpeed);
    Serial.println(leftMotorSpeed);
   
    motor1.drive(rightMotorSpeed);
    motor2.drive(leftMotorSpeed);
    Serial.println(sensors1[0]);
    Serial.println(sensors1[7]);

    if ((sensors1[0] > thr[0]) || (sensors1[7] > thr[7]))
    {
      //Serial.println("Found intersection");
      return ;
      // Found an intersection.
    }
    else if (sensors1[1] < thr[1] && sensors1[2] < thr[2] && sensors1[3] < thr[3] && sensors1[4] < thr[4] && sensors1[5] < thr[5] && sensors1[6] < thr[6])
    {
      // There is no line visible ahead, and we didn't see any
      // intersection. Must be a dead end.
      // Serial.println("condition2");
      return ;
    }
    delay(5);
  }
}

void follow_segment1()
{
  // Fast pid after turn to get quickly align a bot on line
  int Kp = 1;  //1
  int Kd = 10; //10
  for (int j = 0; j < 10; j++) {
    int position = qtra.readLineBlack(sensors1);       // FOR BLACK LINE FOLLOWER JUST REPLACE White WITH Black
    int error =  3500 - position;
    int motorSpeed = Kp * error + Kd * (error - lastError);
    lastError = error;
    int rightMotorSpeed = BaseSpeed - motorSpeed;
    int leftMotorSpeed = BaseSpeed + motorSpeed;
    if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed;
    if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed;
    if (rightMotorSpeed < 0)rightMotorSpeed = 0;
    if (leftMotorSpeed < 0)leftMotorSpeed = 0;
    motor1.drive(rightMotorSpeed);
    motor2.drive(leftMotorSpeed);
    delay(1);
  }
}

// void follow_segment2()
// {
//   //Back moving pid
//   int Kp = 1;  //1
//   int Kd = 50; //50
//   int baseSpeed = 90;
//   int maxSpeed = 90;
//   for (int j = 0; j < 70; j++) {
//     int position = qtra.readLineBlack(sensors1);       // FOR BLACK LINE FOLLOWER JUST REPLACE White WITH Black
//     int error =  3500 - position;
//     int motorSpeed = Kp * error + Kd * (error - lastError);
//     lastError = error;
//     int rightMotorSpeed = baseSpeed + motorSpeed;
//     int leftMotorSpeed = baseSpeed - motorSpeed;
//     if (rightMotorSpeed > maxSpeed ) rightMotorSpeed = maxSpeed;
//     if (leftMotorSpeed > maxSpeed ) leftMotorSpeed = maxSpeed;
//     if (rightMotorSpeed < 0)rightMotorSpeed = 0;
//     if (leftMotorSpeed < 0)leftMotorSpeed = 0;
//     motor1.drive(-rightMotorSpeed);
//     motor2.drive(-leftMotorSpeed);
//     delay(1);
//   }
// }
// void follow_segment3()
// {
//   int Kp = 1  ;
//   int Kd = 10 ; //10
//   for (int j = 0; j < 10; j++) {
//     int position = qtra.readLineBlack(sensors1);       // FOR BLACK LINE FOLLOWER JUST REPLACE White WITH Black
//     int error = 3500 - position;
//     int motorSpeed = Kp * error + Kd * (error - lastError);
//     lastError = error;
//     int rightMotorSpeed = BaseSpeed - motorSpeed;
//     int leftMotorSpeed = BaseSpeed + motorSpeed;
//     if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed;
//     if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed;
//     if (rightMotorSpeed < 0)rightMotorSpeed = 0;
//     if (leftMotorSpeed < 0)leftMotorSpeed = 0;
//     motor1.drive(rightMotorSpeed);
//     motor2.drive(leftMotorSpeed);
//     delay(1);
//   }
// }

char select_turnL(char found_left, char found_straight, char found_right)
{
  // Make a decision about how to turn. The following code
  // Implements a left-hand-on-the-wall strategy, where we always preffer left turn
  if (found_left)
    return 'L';
  else if (found_straight)
    return 'S';
  else if (found_right)
    return 'R';
  else
    return 'B';
}

char select_turnR(char found_right, char found_straight, char found_left)
{
  // Make a decision about how to turn. The following code
  // Implements a right-hand-on-the-wall strategy, Where we always preffer right turn
  if (found_right)
    return 'R';
  else if (found_straight)
    return 'S';
  else if (found_left)
    return 'L';
  else
    return 'B';
}

void turn(char dir)
{
  int line_position;
  // According to dir perform a turn
  switch (dir)
  {
    case'L':
      qtra.readLineBlack(sensors1);
      while (sensors1[0] < thr[0])  ///change
      {
      left(motor1, motor2, 300);
        line_position = qtra.readLineBlack(sensors1);    //Move left until left extreme sensor goes out of the white line comes on black
      }
      qtra.readLineBlack(sensors1);
      while (sensors1[0] > thr[0])
      {
      left(motor1, motor2, 200);
        line_position = qtra.readLineBlack(sensors1);        // Again move left until it get's on white line
      }
      follow_segment1();    // Fast pid after tur n to align bot quickly on line
      brake(motor1, motor2);
      break;
    case'R':
      qtra.readLineBlack(sensors1);
      while (sensors1[7] < thr[7])
      {
      right(motor1, motor2, 300);
        line_position = qtra.readLineBlack(sensors1);   //Move right until left extreme sensor goes out of the white line comes on black
      }
      qtra.readLineBlack(sensors1);
      while (sensors1[7] > thr[7])
      {
      right(motor1, motor2, 200);
        line_position = qtra.readLineBlack(sensors1);         // Again move right until it get's on white line
      }
      follow_segment1();      // Fast pid after turn to align a bot quickly on line
      brake(motor1, motor2);
      break;
    case'B':
      qtra.readLineBlack(sensors1);
      while (sensors1[7] < thr[7])
      {
      right(motor1, motor2, 400);
        line_position = qtra.readLineBlack(sensors1);          // Take u turn using right turn
      }
      qtra.readLineBlack(sensors1);
      while (sensors1[7] > thr[7])
      {
      right(motor1, motor2, 180);
        line_position = qtra.readLineBlack(sensors1);
      }
      follow_segment1();
      brake(motor1, motor2);
      // delay(50);
      // follow_segment2();    // Back moving pid
      // forward(motor1, motor2, 200);
      // delay(40);
      break;
  }
}


void simplify_path()
{
  // Path simplification. The strategy is that whenever we encounter a
  // sequence xBx, we can simplify it by cutting out the dead end.

  if (path_length < 3 || path[path_length - 2] != 'B')      // simplify the path only if the second-to-last turn was a 'B'
    return;
  int total_angle = 0;
  int m;
  // Get the angle as a number between 0 and 360 degrees.
  for (m = 1; m <= 3; m++)
  {
    switch (path[path_length - m])
    {
      case 'R':
        total_angle += 90;
        break;
      case 'L':
        total_angle += 270;
        break;
      case 'B':
        total_angle += 180;
        break;
    }
  }
  // Replace all of those turns with a single one.
  total_angle = total_angle % 360;
  switch (total_angle)
  {
    case 0:
      path[path_length - 3] = 'S';
      break;
    case 90:
      path[path_length - 3] = 'R';
      break;
    case 180:
      path[path_length - 3] = 'B';
      break;
    case 270:
      path[path_length - 3] = 'L';
      break;
  }
  path_length -= 2;
}


