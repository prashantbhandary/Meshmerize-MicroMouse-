#define sw1 26
#define sw2 27
#define led 23
int s1, s2;
void setup() {
  pinMode(sw1, INPUT);  // Set sw1 as input
  pinMode(sw2, INPUT);  // Set sw2 as input
  pinMode(led, OUTPUT); // Set led as output
}
void loop() {
  s1 = digitalRead(sw1);  // Read state of sw1 to switch on the LED
  
  while (s1 == HIGH) {  // Wait for the switch to be released
    s1 = digitalRead(sw1);    
  }
  digitalWrite(led, HIGH);  // Turn LED on
  delay(400);  // Delay to keep LED on for a while

  s2 = digitalRead(sw2);  // Read state of sw2 to switch off the LED
  
  while (s2 == HIGH) {  // Wait for the switch to be released
    s2 = digitalRead(sw2);    
  }
  digitalWrite(led, LOW);  // Turn LED off
}