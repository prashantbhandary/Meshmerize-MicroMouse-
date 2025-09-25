#include <SparkFun_TB6612.h>
#define AIN1 18
#define BIN1 17
#define AIN2 5
#define BIN2 16
#define PWMA 19
#define PWMB 4

#define sw1 26
#define sw2 27
#define led 23
int s1;

Motor motor1 = Motor(BIN1, BIN2, PWMB, 1, 1);
Motor motor2 = Motor(AIN1, AIN2, PWMA, 1, 1);

void setup() {
  pinMode(sw1, INPUT);
  pinMode(sw2,INPUT);
  
  pinMode(led, OUTPUT);
  

   s1 = digitalRead(sw1);  //s1 for running bot
  while (s1 == HIGH)
  {
    s1 = digitalRead(sw1);    
  }
  digitalWrite(led,HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  forward(motor1, motor2, 200);
  delay(4000);
  // back(motor1, motor2, 200);
  // delay(400);
  // brake(motor1, motor2);
  // delay(50);
  // left(motor1, motor2, 200); 
  // delay(4000);
  // right(motor1, motor2, 200);
  // delay(400);
}