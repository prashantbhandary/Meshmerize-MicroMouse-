

#include <SparkFun_TB6612.h>  

#define AIN1 4
#define BIN1 8
#define AIN2 5
#define BIN2 9
#define PWMA 3
#define PWMB 6
// Initialization of the controls
#define sw1 10
#define sw2 11
#define sw3 12 
#define led 7
int s1,s3,s2;


Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, 1, 1);

void setup() {
  pinMode(sw1, INPUT);
  pinMode(sw2,INPUT);

  // pinMode(10,OUTPUT);
  // pinMode(led, OUTPUT);
  // digitalWrite(10,HIGH);

  //  s1 = digitalRead(sw1);  //s1 for running bot
  // while (s1 == HIGH)
  // {
  //   s1 = digitalRead(sw1);    
  // }
  // digitalWrite(2,HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  forward(motor1, motor2, 255);
  // delay(400);
  // back(motor1, motor2, 255);
  // delay(400);
  // brake(motor1, motor2);
  // delay(50);
  // left(motor1, motor2, 200); 
  // delay(400);
  // right(motor1, motor2, 400);
  // delay(400);
}